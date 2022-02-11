//! The vector generator controller proceeds as follows:
//!
//! It maintains two buffers representing instructions to draw a scene. At any time, one buffer is
//! being written to while the other buffer is being displayed (calling [double
//! buffering](https://wiki.osdev.org/Double_Buffering)).
//!
//! The buffer gets written over some protocol (likely UART) connection from the main controller.
//! It parses input instructions according to the [VectorCommand] enum's binary format (described
//! in the `VectorCommand` documentation) and stores them in a local array. When it receives a
//! [VectorCommand::Buffer] command, it begins to draw the vectors specified by the argument, and
//! switches to writing the other commands.
//!
//! When displaying a buffer, it sequentially does each command, beginning at the start of the
//! buffer and proceeding until it reaches a [VectorCommand::Halt] instruction. The contents of the
//! buffer are preserved between iterations. Thus, if the same frame should be drawn again, the
//! controller can be sent another [VectorCommand::Buffer] instruction without any writes to the
//! buffer first. Also, when a frame is written, it should always end in a [VectorCommand::Halt]
//! instruction, because there may be some other instructions sitting from a previous time.

#![no_std]
#![feature(mixed_integer_ops)]

use either::Either::{self, Left, Right};
use riscv::asm::wfi;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

/// The commands which the vector generator controller can execute.
///
/// When drawing a buffer, it maintains an internal position on the screen in (x,y) coordinates.
/// The screen is indexed with `(0,0)` in the top-left of the screen, and `(1023,1023)` in the
/// bottom-right corner. Because the electron beam can not make sudden movements, commands begin
/// drawing at the presently stored location, and then the location at which they stop drawing
/// becomes the start location for the next command.
///
/// The binary encoding is variable-width: the size of the instruction must be determined by the
/// leading byte, but it can vary from 1 to 4 bytes. Each of the variants of this enum represents a
/// command which it can do, and is documented with its binary format. Any leading bytes not
/// matching one of those has no present meaning and should not be sent, but new instructions may
/// be added later which use those parts of the instruction space.
pub enum VectorCommand {
    /// Draw a line to the given absolute coordinates, with the specified brightness.
    ///
    /// Note that, if you wish to move from one location to another on the screen without drawing a
    /// line, you should invoke this command with a brightness of zero.
    ///
    /// Also, a buffer should begin with the above movement to a fixed location because it will
    /// otherwise keep the positioning at which the previous buffer ended.
    ///
    /// This command begins with `10` to indicate this command, followed by 10 bits for the x
    /// coordinate of the next endpoint, 10 bits for the y coordinate, and 10 bits for the
    /// intensity of the line to draw, for a total instruction size of 4 bytes.
    Line {
        end_x: u16,
        end_y: u16,
        brightness: u16,
    },
    /// Draw a line to the given relative coordinates, with the specified brightness. This is like
    /// [VectorCommand::Line] except the x and y are relative to the present location, and not
    /// absolute on the screen.
    ///
    /// This command begins with `11` to indicate this command, followed by 10 bits for the x
    /// coordinate of the next endpoint, 10 bits for the y coordinate, and 10 bits for the
    /// intensity of the line to draw, for a total instruction size of 4 bytes.
    RelLine {
        delta_x: i16,
        delta_y: i16,
        brightness: u16,
    },
    /// Begin drawing the specified buffer and writing the other buffer. This command begins with
    /// `0110_00` to specify the command, followed by two bits to determine [which buffer
    /// instruction](BufferInstructionType) is requested. `00` corresponds to [drawing buffer
    /// A](BufferInstructionType::DrawA), `01` corresponds to [drawing buffer
    /// B](BufferInstructionType::DrawB), `10` corresponds to [drawing the most recently drawn
    /// buffer](BufferInstructionType::DrawSame), and `11` corresponds to [drawing the least
    /// recently drawn buffer](BufferInstructionType::DrawSwitch).
    ///
    /// Unlike the other [commands](VectorCommand), this one immediately takes effect instead of
    /// being written to the buffer and waiting until later.
    Buffer(BufferInstructionType),
    /// End drawing the buffer.
    ///
    /// This command is the null byte.
    Halt,
}
/// An enum representing which buffer the controller should draw.
///
/// In normal use, you probably want to use [DrawSwitch](BufferInstructionType::DrawSwitch), as it
/// will draw the buffer which has just been written to, but the others are provided for
/// completeness.
///
/// See [VectorCommand::Buffer] for the bit encodings of each
pub enum BufferInstructionType {
    /// Draw buffer A, and set buffer B for writing.
    ///
    /// This resets the indices of both buffers to the beginning.
    DrawA,
    /// Draw buffer B, and set buffer A for writing.
    ///
    /// This resets the indices of both buffers to the beginning.
    DrawB,
    /// Draw the most recently drawn buffer, which will repeat the same image as previously drawn.
    ///
    /// When executed, this resets the position of the buffer being drawn (so it draws the entire
    /// buffer) but the position of the buffer being written is kept, so that it continues to be
    /// appended from the same location as before.
    DrawSame,
    /// Draw the buffer which is least recently drawn, which is also the one most recently written.
    ///
    /// When executed, this instruction also resets the position of both buffers to the beginning.
    DrawSwitch,
}
impl VectorCommand {
    /// Given a byte, return either:
    /// - The instruction encoded by this byte, if the bytes encodes an instruction in its
    ///   entirety.
    /// - The number of bytes remaining in the instruction which begins with this byte, if this
    ///   byte does not encode an instruction in its entirety
    /// - Right(0) if this is not a valid instruction
    ///
    /// This function will be used in interpreting the bytes we receive from the main processor, to
    /// decide whether we append it to the instruction buffer or if we begin drawing.
    pub fn parse_byte(byte: u8) -> Either<Self, usize> {
        Left(match byte {
            0 => VectorCommand::Halt,
            x if (x & 0xFC == 0x60) => {
                let buf = match x & 0x03 {
                    0 => BufferInstructionType::DrawA,
                    1 => BufferInstructionType::DrawB,
                    2 => BufferInstructionType::DrawSame,
                    3 => BufferInstructionType::DrawSwitch,
                    _ => unreachable!(),
                };
                VectorCommand::Buffer(buf)
            }
            x if (x & 0xC0 == 0x80) => return Right(3),
            x if (x & 0xC0 == 0xC0) => return Right(3),
            _ => return Right(0),
        })
    }
    /// Given a buffer and a starting index into the buffer, return the [VectorCommand]
    /// representing the command at the given index, and update the index to the index of the next
    /// command in the buffer
    pub fn from_buffer(buffer: &[u8], index: &mut usize) -> Option<Self> {
        Some(match buffer[*index] {
            // Halt
            0 => {
                *index += 1;
                VectorCommand::Halt
            }
            // Draw Buffer
            x if (x & 0xFC == 0x60) => {
                let buf = match x & 0x03 {
                    0 => BufferInstructionType::DrawA,
                    1 => BufferInstructionType::DrawB,
                    2 => BufferInstructionType::DrawSame,
                    3 => BufferInstructionType::DrawSwitch,
                    _ => unreachable!(),
                };
                *index += 1;
                VectorCommand::Buffer(buf)
            }
            x if (x & 0xC0 == 0x80) => {
                let end_x: u16 =
                    (buffer[*index] as u16 & 0x3F) << 4 | (buffer[*index + 1] as u16 & 0xF0);
                let end_y: u16 =
                    (buffer[*index + 1] as u16 & 0x0F) << 6 | (buffer[*index + 2] as u16 & 0xFC);
                let brightness: u16 =
                    (buffer[*index + 2] as u16 & 0x03) << 8 | (buffer[*index + 3] as u16);
                *index += 4;
                VectorCommand::Line {
                    end_x,
                    end_y,
                    brightness,
                }
            }
            x if (x & 0xC0 == 0xC0) => {
                let x: u16 =
                    (buffer[*index] as u16 & 0x3F) << 4 | (buffer[*index + 1] as u16 & 0xF0);
                let delta_x = if x & 0x200 == 0 {
                    x as i16
                } else {
                    (x | 0xFC00) as i16
                };
                let y: u16 =
                    (buffer[*index + 1] as u16 & 0x0F) << 6 | (buffer[*index + 2] as u16 & 0xFC);
                let delta_y = if y & 0x200 == 0 {
                    y as i16
                } else {
                    (y | 0xFC00) as i16
                };
                let brightness: u16 =
                    (buffer[*index + 2] as u16 & 0x03) << 8 | (buffer[*index + 3] as u16);
                *index += 4;
                VectorCommand::RelLine {
                    delta_x,
                    delta_y,
                    brightness,
                }
            }
            _ => return None,
        })
    }
}

const VECTOR_BUFFER_LENGTH: usize = 4096;
// State which is needed in both interrupt handlers
static mut BUFFER_A: [u8; VECTOR_BUFFER_LENGTH] = [0; VECTOR_BUFFER_LENGTH];
static mut BUFFER_A_INDEX: usize = 0;
static mut BUFFER_B: [u8; VECTOR_BUFFER_LENGTH] = [0; VECTOR_BUFFER_LENGTH];
static mut BUFFER_B_INDEX: usize = 0;
static mut DRAWING_BUFFER_A: bool = true;
static mut DRAWING_HALTED: bool = true;
// State which is only needed when handling a gpio interrupt (drawing output)
static mut DRAW_X_POS: u16 = 0;
static mut DRAW_Y_POS: u16 = 0;
// State which is only needed when handling a uart interrupt (reading instructions)
static mut BYTES_LEFT_IN_CURRENT_INSTRUCTION: usize = 0;

/// Handle a UART interrupt, which means that we've received a byte from the main CPU.
///
/// The argument should be the byte that we've received.
pub fn handle_uart_interrupt(byte: u8) {
    let (write_buffer, write_index) = unsafe {
        if DRAWING_BUFFER_A {
            (&mut BUFFER_B, &mut BUFFER_B_INDEX)
        } else {
            (&mut BUFFER_A, &mut BUFFER_A_INDEX)
        }
    };
    if unsafe { BYTES_LEFT_IN_CURRENT_INSTRUCTION } > 0 {
        unsafe { BYTES_LEFT_IN_CURRENT_INSTRUCTION -= 1 };
        write_buffer[*write_index] = byte;
        *write_index += 1;
    } else {
        match VectorCommand::parse_byte(byte) {
            Left(VectorCommand::Buffer(instruction)) => match instruction {
                BufferInstructionType::DrawA => unsafe {
                    DRAWING_BUFFER_A = true;
                    DRAWING_HALTED = false;
                    BUFFER_A_INDEX = 0;
                    BUFFER_B_INDEX = 0;
                },
                BufferInstructionType::DrawB => unsafe {
                    DRAWING_BUFFER_A = false;
                    DRAWING_HALTED = false;
                    BUFFER_A_INDEX = 0;
                    BUFFER_B_INDEX = 0;
                },
                BufferInstructionType::DrawSame => unsafe {
                    DRAWING_HALTED = false;
                    if DRAWING_BUFFER_A {
                        BUFFER_A_INDEX = 0;
                    } else {
                        BUFFER_B_INDEX = 0;
                    }
                },
                BufferInstructionType::DrawSwitch => unsafe {
                    DRAWING_BUFFER_A = !DRAWING_BUFFER_A;
                    DRAWING_HALTED = false;
                    BUFFER_A_INDEX = 0;
                    BUFFER_B_INDEX = 0;
                },
            },
            Left(_) => {
                write_buffer[*write_index] = byte;
                *write_index += 1;
            }
            Right(count) => {
                write_buffer[*write_index] = byte;
                *write_index += 1;
                unsafe {
                    BYTES_LEFT_IN_CURRENT_INSTRUCTION = count;
                }
            }
        }
    }
}

/// Handle a GPIO interrupt from the ramp, which means using the GPIO pins to update the DACs to
/// output the next line.
///
/// The argument should be true if the ramp input is low, and false if the ramp input is high
pub fn handle_gpio_interrupt(input_low: bool) {
    if unsafe { DRAWING_HALTED } {
        return;
    }
    let (draw_buffer, draw_index) = unsafe {
        if DRAWING_BUFFER_A {
            (&BUFFER_A, &mut BUFFER_A_INDEX)
        } else {
            (&BUFFER_B, &mut BUFFER_B_INDEX)
        }
    };
    match VectorCommand::from_buffer(draw_buffer, draw_index) {
        Some(VectorCommand::Halt) => unsafe {
            DRAWING_HALTED = true;
            // TODO set the DACs in a state where the beam is off and we can quickly resume drawing
        },
        Some(VectorCommand::Line {
            end_x,
            end_y,
            brightness: _,
        }) => {
            // TODO write the outputs to the appropriate DAC slots
            unsafe {
                DRAW_X_POS = end_x;
                DRAW_Y_POS = end_y;
            }
        }
        Some(VectorCommand::RelLine {
            delta_x,
            delta_y,
            brightness: _,
        }) => {
            let end_x = unsafe { DRAW_X_POS }.saturating_add_signed(delta_x);
            let end_y = unsafe { DRAW_Y_POS }.saturating_add_signed(delta_y);
            // TODO write the outputs to the appropriate DAC slots
            unsafe {
                DRAW_X_POS = end_x;
                DRAW_Y_POS = end_y;
            }
        }
        Some(VectorCommand::Buffer(_)) => {
            // We shouldn't be able to reach this, as these commands shouldn't be put in the buffer
            unreachable!()
        }
        None => {
            // Default case: if unrecognized instruction, we should probably given an error
            // indication???
            // For now, we just skip to the next instruction
            handle_gpio_interrupt(input_low);
        }
    }
}

#[no_mangle]
extern "C" fn main() -> i32 {
    // TODO: Set up global memory for use in interrupt handlers and set up interrupt handlers
    let _gpio = unsafe { sifive_fe310_g002::gpio::GPIO.as_mut().unwrap() };
    let _uart = unsafe { sifive_fe310_g002::uart::UART0.as_mut().unwrap() };
    loop {
        // In main, we just wait in a loop, because we now only need to respond to interrupts
        unsafe { wfi() }
    }
}
