//! The vector generator controller proceeds as follows:
//!
//! It maintains two buffers representing instructions to draw a scene. At any time, one buffer is
//! being written to while the other buffer is being displayed (calling [double
//! buffering](https://wiki.osdev.org/Double_Buffering)).
//!
//! The buffer gets written over some (tbd which) connection from the main controller. It parses
//! input instructions according to the [VectorCommand] enum's binary format (described in the
//! `VectorCommand` documentation) and stores them in a local array. When it receives a
//! [VectorCommand::Buffer] command, it begins to draw the vectors specified by the argument, and
//! switches to writing the other commands.
//!
//! When displaying a buffer, it sequentially does each command, beginning at the start of the
//! buffer and proceeding until it reaches a [VectorCommand::Halt] instruction.

#![no_std]

#[repr(C)]
struct GpioPin {
    input_val: u32,
    input_en: u32,
    output_en: u32,
    output_val: u32,
    pue: u32,
    ds: u32,
    rise_ie: u32,
    rise_ip: u32,
    fall_ie: u32,
    fall_ip: u32,
    high_ie: u32,
    high_ip: u32,
    low_ie: u32,
    low_ip: u32,
    out_xor: u32,
}

const GPIO_ADDRESS: usize = 0x1001_2000;

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
    /// [Line] except the x and y are relative to the present location, and not absolute on the
    /// screen.
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
    /// `0000_001` to specify the command, followed by a `0` to indicate that buffer 0 is being
    /// drawn or a `1` to indicate that buffer 1 is being drawn.
    Buffer(u8),
    /// End drawing the buffer.
    ///
    /// This command is the null byte.
    Halt,
}

#[no_mangle]
extern "C" fn main() -> i32 {
    // For now, we just blink an LED
    let gpio = unsafe { (GPIO_ADDRESS as *mut GpioPin).as_mut() }.unwrap();
    gpio.output_en |= 1 << 5;
    loop {
        gpio.output_val ^= 1 << 5;
    }
}
