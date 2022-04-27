use std::cmp;
use std::cmp::*;
use std::env;
use std::io::Write;
use std::error::Error;
use std::f32::consts::PI;
use std::fs::File;
use std::io;
use std::io::Read;
use std::sync::mpsc;
use std::sync::mpsc::Receiver;
use std::sync::mpsc::TryRecvError;
use std::time::Duration;
use std::time::Instant;
use std::{thread, time};
use clap::{App, Arg};

use glutin::error;
use glutin::{
    dpi::{LogicalSize, PhysicalSize},
    event::{Event, WindowEvent},
    event_loop::{ControlFlow, EventLoop},
    window::WindowBuilder,
    ContextBuilder,
};

use femtovg::{
    renderer::OpenGl, Align, Baseline, Canvas, Color, FillRule, FontId, ImageFlags, ImageId,
    LineCap, LineJoin, Paint, Path, Renderer, Solidity,
};

use os_pipe::PipeReader;
use os_pipe::PipeWriter;

struct Options {
    pub show_axis: bool,
    pub speed_of_motor: f32,
    pub fade_rate: f32,
    pub frame_delay: f32,
}

#[derive(Debug, Clone, Copy, PartialEq)]
enum Draw {
    None,
    DrawA,
    DrawB,
    DrawSame,
    DrawSwitch,
}

#[derive(Debug, Clone, Copy)]
struct VCommand {
    pub x: f32,
    pub y: f32,
    pub brightness: u16,
    pub relative: bool,
    pub halt: bool,
    pub draw: Draw,
}

impl VCommand {
    pub fn new(x: f32, y: f32, brightness: u16, relative: bool, halt: bool, draw: Draw) -> Self {
        Self {
            x,
            y,
            brightness,
            relative,
            halt,
            draw,
        }
    }

    pub fn is_relative(&self) -> bool {
        self.relative
    }

    pub fn new_from_string(string: &str) -> Result<Self, Box<dyn Error>> {
        if string.to_lowercase() == "halt" {
            return Ok(Self::new(0.0, 0.0, 0, false, true, Draw::None));
        } else {
            let mut parts = string.split_whitespace();
            let command = parts.next().ok_or("No command")?;
            let relative;

            if command.to_lowercase() == "rvec" {
                relative = true;
            } else {
                relative = false;
            }

            let x = parts.next().ok_or("No x value")?.parse::<f32>()?;
            let y = parts.next().ok_or("No y value")?.parse::<f32>()?;
            let brightness = parts.next().ok_or("No brightness value")?.parse::<u16>()?;

            return Ok(Self::new(x, y, brightness, relative, false, Draw::None));
        }
    }

    /// Creates a new VCommand from a string, piped in from the game
    /// # Spec:
    /// ## halt: 0
    /// ## vec: 10
    /// - Draws a absolute vector
    /// - 10 bits x,y,brightness
    /// ## rvev: 11
    /// - Draws a relative vector
    /// - 10 bits x,y,brightness
    /// ## draw: 01
    /// - Sets draw command
    /// - 00 - Draw buffer A
    /// - 01 - Draw buffer B
    /// - 10 - Draw current buffer
    /// - 11 - Draw and then switch
    /// # Examples:
    /// - "0" - Marks the end of a buffer. Any more commands will erase that buffer
    /// - "10000001111100000111111111111111" - Draws a vector to 31,31 with brightness 1023/1023
    /// - "01000001101100000110111111111111" - Draws a vector to 27,27,     he current vector
    pub fn new_from_pipe(bits: String) -> Result<Self, Box<dyn Error>> {
        let mut parts = bits.split_whitespace();
        let bits = parts.next().ok_or("No command")?.to_string();

        if &bits == "0" {
            return Ok(Self::new(0.0, 0.0, 0, false, true, Draw::None));
        }

        let command_type = &bits[0..2];

        // It's a absolute command
        if command_type == "10" || command_type == "11" {
            let x = signed_binary_conversion(&bits[2..12]);
            let y = signed_binary_conversion(&bits[12..22]);

            let brightness = u16::from_str_radix(&bits[22..32], 2)?;

            let relative = command_type == "11";

            return Ok(Self::new(x, y, brightness, relative, false, Draw::None));

        } else if command_type == "01" {
            // It's a draw command
            let draw = match &bits[2..4] {
                "00" => Draw::DrawA,
                "01" => Draw::DrawB,
                "10" => Draw::DrawSame,
                "11" => Draw::DrawSwitch,
                _ => {
                    let error_msg = format!("Invalid draw command: {}", bits);
                    return Err(Box::new(io::Error::new(io::ErrorKind::InvalidData, error_msg)));
                },
            };

            return Ok(Self::new(0.0, 0.0, 0, false, false, draw));
        } else {
            let error_msg = format!("Invalid command: {}", bits);
            return Err(Box::new(io::Error::new(io::ErrorKind::InvalidData, error_msg)));
        }
    }
}

pub fn signed_binary_conversion(binary: &str) -> f32 {
    if binary.starts_with("0") {
        return u16::from_str_radix(&binary, 2).unwrap() as f32;
    } else {
        let width: u16 = binary.len() as u16;

        let raw_value = u16::from_str_radix(&binary, 2).unwrap();

        return -((2_u16.pow(width as u32) - raw_value) as f32);
    }
}

// A compiled vector line using absolute coordinates.
#[derive(Debug, Clone)]
struct VLine {
    pub x1: f32,
    pub y1: f32,
    pub x2: f32,
    pub y2: f32,
    pub brightness: u16,
    pub draw_progress: f32,
    last_fade: Instant,
}

impl VLine {
    pub fn new(x1: f32, y1: f32, x2: f32, y2: f32, brightness: u16) -> Self {
        Self {
            x1,
            y1,
            x2,
            y2,
            brightness,
            draw_progress: 0.0,
            last_fade: Instant::now(),
        }
    }

    pub fn from_command(command: &VCommand, current_x: f32, current_y: f32) -> Self {
        if command.is_relative() {
            Self::new(
                current_x,
                current_y,
                current_x + command.x,
                current_y + command.y,
                command.brightness,
            )
        } else {
            Self::new(current_x, current_y, command.x, command.y, command.brightness)
        }
    }

    fn fade(&mut self) {
        if self.brightness > 0 {
            self.brightness -= 1;
        }
    }

    pub fn fade_if_old(&mut self, now: Instant, fade_rate: f32) {
        if now.duration_since(self.last_fade) > std::time::Duration::from_secs_f32(fade_rate) {
            self.fade();
            self.last_fade = now;
        }
    }

    pub fn is_dead(&self) -> bool {
        self.brightness == 0
    }

    pub fn as_canvas_coords(&self, canvas_width: f32, canvas_height: f32) -> (f32, f32, f32, f32) {
        let x1 = ((self.x1 / MAX_COORD) * canvas_width as f32) + (canvas_width / 2.);
        let x2 = ((self.x2 / MAX_COORD) * canvas_width as f32) + (canvas_width / 2.);

        let y1 = ((-self.y1 / MAX_COORD) * canvas_height as f32) + (canvas_height / 2.);
        let y2 = ((-self.y2 / MAX_COORD) * canvas_height as f32) + (canvas_height / 2.);

        (x1, y1, x2, y2)
    }

    pub fn distance(&self, canvas_width: f32, canvas_height: f32) -> f32 {
        let coords = self.as_canvas_coords(canvas_width, canvas_height);
        (((coords.2 - coords.0).powf(2.)) + ((coords.3 - coords.1).powf(2.))).sqrt()
    }

    pub fn calculate_draw_time(&self, speed: f32, canvas_width: f32, canvas_height: f32) -> f32 {
        self.distance(canvas_width, canvas_height) / speed
    }

    pub fn get_progress(&self) -> f32 {
        self.draw_progress
    }

    pub fn update_progress(
        &mut self,
        last_frame_time: Instant,
        speed: f32,
        canvas_width: f32,
        canvas_height: f32,
    ) -> f32 {
        if self.draw_progress != 1. {
            self.draw_progress = (1. as f32).min(
                speed * last_frame_time.elapsed().as_secs_f32()
                    / self.distance(canvas_width, canvas_height),
            );
        }
        self.draw_progress
    }

    pub fn finish(&mut self) {
        self.draw_progress = 1.;
    }

    pub fn get_end_point(&self) -> (f32, f32) {
        return (self.x2, self.y2);
    }
}

/// Max value for signed 10-bit integer.
const MAX_COORD: f32 = 1023.0;
/// Min value for signed 10-bit integer.
const MIN_COORD: f32 = -1023.0;

fn open_file(path: &str) -> Result<Vec<VCommand>, Box<dyn Error>> {
    let mut file = std::fs::File::open(path)?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;

    contents = contents.to_lowercase();

    let mut commands = Vec::new();
    for line in contents.lines() {
        commands.push(VCommand::new_from_string(line)?);
    }
    Ok(commands)
}

fn main() {
    // Speed of x/y motors in coords per second
    let mut speed_of_motor: f32 = 1500.0;
    // Seconds it takes to fade one brightness value
    let mut fade_rate: f32 = 0.01;
    // Frame delay in seconds
    let mut frame_delay: f32 = 0.5;

    let mut fancy_mode = false;

    let matches = App::new("Vector Generator")
        .version("1.0")
        .author("Ethan Vazquez <edv121@outlook.com>")
        .about("Simple vector generator")
        .arg(
            Arg::with_name("input")
                .short("i")
                .long("input")
                .takes_value(true)
                .help("Sets the input file to use"),
        )
        .arg(
            Arg::with_name("speed")
                .short("s")
                .long("speed")
                .takes_value(true)
                .help("Speed of the motors in pixels per second. Default is 1500"),
        )
        .arg(
            Arg::with_name("fade")
                .short("f")
                .long("fade")
                .takes_value(true)
                .help("Fade rate in seconds. Default is 0.01"),
        )
        .arg(
            Arg::with_name("delay")
                .short("d")
                .long("delay")
                .takes_value(true)
                .help("Frame delay in seconds. Default is 0.5"),
        )
        .arg(
            Arg::with_name("show_axis")
                .short("a")
                .long("show-axis")
                .takes_value(false)
                .help("Show the axis"),
        )
        .arg(
            Arg::with_name("realistic_mode")
                .short("r")
                .long("realistic-mode")
                .takes_value(false)
                .help("Sets speed, fade, and delay to realistic values. Is overridden by speed, fade, and delay arguments"),
        )
        .arg(
            Arg::with_name("pipe_mode")
                .short("p")
                .long("pipe-mode")
                .takes_value(false)
                .help("Wait for input from pipes"),
        )
        .arg(
            Arg::with_name("fancy_mode")
                .short("n")
                .long("fancy-mode")
                .takes_value(false)
                .help("Use fancy mode"),
        )
        .get_matches();

    let show_axis = matches.is_present("show_axis");

    if matches.is_present("realistic_mode") {
        speed_of_motor = 150000.0;
        fade_rate = 0.001;
        frame_delay = 1. / 30.;
    }

    if let Some(speed) = matches.value_of("speed") {
        speed_of_motor = speed.parse::<f32>().expect("Speed must be a number");
    }

    if let Some(fade) = matches.value_of("fade") {
        fade_rate = fade.parse::<f32>().expect("Fade rate must be a number");
    }

    if let Some(delay) = matches.value_of("delay") {
        frame_delay = delay.parse::<f32>().expect("Frame delay must be a number");
    }

    if matches.is_present("fancy_mode") {
        fancy_mode = true;
    }

    let pipe_mode = matches.is_present("pipe_mode");

    let options = Options {
        show_axis,
        speed_of_motor,
        fade_rate,
        frame_delay,
    };

    if pipe_mode {
        let (mut reader, writer) = os_pipe::pipe().expect("Failed to create pipe");

        start_window(options, None, true);
    } else {
        let input_file = matches.value_of("input").expect("Input file invalid");

        println!("Opening file...");

        let mut commands_to_process = open_file(input_file).expect("Could not open file");

        println!("Opened file with {} commands", commands_to_process.len());

        //for command in commands_to_process.iter_mut() {
        //println!("{:?}", command);
        //}

        start_window(options, Some(commands_to_process), false);
    }
}

struct Buffers {
    buffer_a: Vec<VLine>,
    buffer_b: Vec<VLine>,
    buffer_a_halt: bool,
    buffer_b_halt: bool,
}

impl Buffers {
    pub fn new() -> Buffers {
        Buffers {
            buffer_a: Vec::new(),
            buffer_b: Vec::new(),
            buffer_a_halt: false,
            buffer_b_halt: false,
        }
    }

    pub fn get_buffer(&self, select: u8) -> Vec<VLine> {
        if select == 0 {
            self.buffer_a.clone()
        } else {
            self.buffer_b.clone()
        }
    }

    pub fn set_buffer(&mut self, select: u8, buffer: Vec<VLine>) {
        if select == 0 {
            self.buffer_a = buffer;
        } else {
            self.buffer_b = buffer;
        }
    }

    pub fn add_to_buffer(&mut self, select: u8, line: VLine) {
        if select == 0 {
            self.buffer_a.push(line);
        } else {
            self.buffer_b.push(line);
        }
    }

    pub fn clear_buffer(&mut self, select: u8) {
        if select == 0 {
            self.buffer_a.clear();
        } else {
            self.buffer_b.clear();
        }
    }

    pub fn set_halt(&mut self, select: u8, halt: bool) {
        if select == 0 {
            self.buffer_a_halt = halt;
        } else {
            self.buffer_b_halt = halt;
        }
    }

    pub fn get_halt(&self, select: u8) -> bool {
        if select == 0 {
            self.buffer_a_halt
        } else {
            self.buffer_b_halt
        }
    }
}

fn start_window(
    options: Options,
    instructions: Option<Vec<VCommand>>,
    pipe_mode: bool,
) {
    // Boilerplate window creation
    let window_size = glutin::dpi::PhysicalSize::new(800, 800);
    let el = EventLoop::new();
    let wb = WindowBuilder::new()
        .with_inner_size(glutin::dpi::PhysicalSize::new(800 as u32, 800 as u32))
        .with_title("Vector Generator");

    let windowed_context = ContextBuilder::new()
        .with_vsync(false)
        .build_windowed(wb, &el)
        .unwrap();
    let windowed_context = unsafe { windowed_context.make_current().unwrap() };

    let renderer = OpenGl::new(|s| windowed_context.get_proc_address(s) as *const _)
        .expect("Cannot create renderer");
    let mut canvas = Canvas::new(renderer).expect("Cannot create canvas");

    canvas.set_size(
        window_size.width as u32,
        window_size.height as u32,
        windowed_context.window().scale_factor() as f32,
    );


    let mut commands_to_run = Vec::new();
    let stdin_channel = spawn_stdin_channel();

    let mut current_x = 0.0;
    let mut current_y = 0.0;

    let mut buffers = Buffers::new();
    let mut buffer_select = 0;
    let mut file = File::create("log.txt").expect("Could not create log file");

    el.run(move |event, _, control_flow| {
        *control_flow = ControlFlow::Poll;

        match event {
            Event::LoopDestroyed => return,
            Event::WindowEvent { ref event, .. } => match event {
                WindowEvent::Resized(physical_size) => {
                    windowed_context.resize(*physical_size);
                }
                WindowEvent::CloseRequested => *control_flow = ControlFlow::Exit,
                _ => (),
            },
            Event::RedrawRequested(_) => {
                let dpi_factor = windowed_context.window().scale_factor();
                let size = windowed_context.window().inner_size();
                canvas.set_size(size.width as u32, size.height as u32, dpi_factor as f32);
                canvas.clear_rect(
                    0,
                    0,
                    size.width as u32,
                    size.height as u32,
                    Color::rgbf(0.0, 0.0, 0.0),
                );

                // Draw ui
                if options.show_axis {
                    draw_ui(&mut canvas);
                }

                if buffers.get_halt(buffer_select) {
                    (current_x, current_y) = draw_buffer(&mut canvas, &buffers.get_buffer(buffer_select)).unwrap_or((current_x, current_y));
                }

                if pipe_mode {
                    match stdin_channel.try_recv() {
                        Ok(buffer) => {
                            let command = VCommand::new_from_pipe(buffer).expect("Could not parse command");
                            commands_to_run.push(command);
                        }
                        Err(TryRecvError::Empty) => {
                        }
                        Err(TryRecvError::Disconnected) => panic!("Channel disconnected"),
                    }
                } else {
                    commands_to_run = instructions.as_ref().unwrap().to_vec();
                }

                while !commands_to_run.is_empty() {
                    let new_command = commands_to_run.remove(0);
                

                    // First, draw type
                    match new_command.draw {
                        Draw::DrawA => {
                            buffer_select = 0;
                        }

                        Draw::DrawB => {
                            buffer_select = 1;
                        }

                        Draw::DrawSwitch => {
                            buffer_select = (buffer_select + 1) % 2;
                        }

                        Draw::DrawSame => {

                        }

                        Draw::None => {
                            // If it's not halt, then add line to current buffer
                            if new_command.halt {
                                buffers.set_halt(buffer_select, true);
                                file.write_all(format!("{:?}\n", &buffers.get_buffer(buffer_select)).as_bytes()).expect("Could not write to log file");
                                file.flush().expect("Could not flush log file");
                            } else {
                                // Clear if it's halted previously
                                if buffers.get_halt(buffer_select) {
                                    buffers.clear_buffer(buffer_select);
                                    buffers.set_halt(buffer_select, false);
                                }

                                let mut new_line = VLine::from_command(&new_command, current_x, current_y);

                                new_line.finish();

                                current_x = new_line.x2;
                                current_y = new_line.y2;

                                buffers.add_to_buffer(buffer_select, new_line);

                                (current_x, current_y) = draw_buffer(&mut canvas, &buffers.get_buffer(buffer_select)).unwrap_or((current_x, current_y));                                
                            }                            
                        }
                    }
                }
                
                canvas.save();
                canvas.reset();
                canvas.restore();
                canvas.flush();
                windowed_context.swap_buffers().unwrap();
            }
            Event::MainEventsCleared => windowed_context.window().request_redraw(),
            _ => (),
        }
    });
}

fn draw_buffer<T: Renderer>(canvas: &mut Canvas<T>, buffer: &Vec<VLine>) -> Option<(f32, f32)> {
    for line in buffer {
        draw_vector_line(canvas, line);
    }

    if buffer.is_empty() {
        None
    } else {
        Some(buffer.last().unwrap().get_end_point())
    }
}

fn draw_ui<T: Renderer>(canvas: &mut Canvas<T>) {
    let mut r = || {
        // Draw axis lines as a dark blue
        let mut p = Path::new();

        let mut paint = Paint::color(Color::rgbf(0.0, 0.0, 0.5));

        paint.set_line_width(1.);
        paint.set_anti_alias(true);
        paint.set_line_cap(LineCap::Round);
        p.move_to(canvas.width() / 2., 0.);
        p.line_to(canvas.width() / 2., canvas.height());
        p.move_to(0., canvas.height() / 2.);
        p.line_to(canvas.width(), canvas.height() / 2.);

        canvas.stroke_path(&mut p, paint);
    };

    r();
}

fn draw_vector_line<T: Renderer>(canvas: &mut Canvas<T>, line: &VLine) {
    let mut r = |x1: f32, y1: f32, x2: f32, y2: f32, brightness: u16| {
        // First, translate x/y to screen coordinates
        // knowing that the origin is in the top left corner
        // and the y axis is inverted

        // Shift over according to the canvas size
        // and max/min 12 bit values
        let translated_coords =
            line.as_canvas_coords(canvas.width() as f32, canvas.height() as f32);

        let x1 = translated_coords.0;
        let y1 = translated_coords.1;
        let x2 = translated_coords.2;
        let y2 = translated_coords.3;

        let mut p = Path::new();
        // Draw line according to the progress of the line
        let progress = line.get_progress();

        let x: f32 = x2 - x1;
        let y: f32 = y2 - y1;
        let angle = y.atan2(x);

        canvas.translate(x1, y1);
        canvas.rotate(angle);
        p.move_to(0., 0.);
        p.line_to(
            line.distance(canvas.width() as f32, canvas.height() as f32) * progress,
            0.,
        );
        let mut paint = Paint::color(Color::rgbf(
            brightness as f32 / 1023.,
            brightness as f32 / 500.,
            brightness as f32 / 500.,
        ));
        paint.set_line_width(2.);
        paint.set_anti_alias(true);
        paint.set_line_cap(LineCap::Round);

        canvas.stroke_path(&mut p, paint);

        canvas.rotate(-angle);
        canvas.translate(-x1, -y1);
    };

    r(line.x1, line.y1, line.x2, line.y2, line.brightness);
}

fn spawn_stdin_channel() -> Receiver<String> {
    let (tx, rx) = mpsc::channel::<String>();
    thread::spawn(move || loop {
        let mut buffer = String::new();
        io::stdin().read_line(&mut buffer).unwrap();
        tx.send(buffer).unwrap();
    });
    rx
}
