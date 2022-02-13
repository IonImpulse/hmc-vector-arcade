use either::Either::{self, Left, Right};

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
