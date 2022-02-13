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

mod command;

pub use command::{BufferInstructionType, VectorCommand};
use either::Either::{Left, Right};
use riscv::asm::wfi;
use sifive_fe310_g002::{
    gpio::{GpioControls, GPIO},
    uart::{UartControls, UART0},
};

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
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
    loop {
        // In main, we just wait in a loop, because we now only need to respond to interrupts
        unsafe { wfi() }
    }
}
