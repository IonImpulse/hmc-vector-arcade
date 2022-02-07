use std::cmp;
use std::cmp::*;
use std::env;
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

#[derive(Debug, Clone, Copy)]
struct VCommand {
    pub x: f32,
    pub y: f32,
    pub brightness: u8,
    pub relative: bool,
    pub halt: bool,
}

impl VCommand {
    pub fn new(x: f32, y: f32, brightness: u8, relative: bool, halt: bool) -> Self {
        Self {
            x,
            y,
            brightness,
            relative,
            halt,
        }
    }

    pub fn is_relative(&self) -> bool {
        self.relative
    }

    pub fn new_from_string(string: &str) -> Result<Self, Box<dyn Error>> {
        if string.to_lowercase() == "halt" {
            return Ok(Self::new(0.0, 0.0, 0, false, true));
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
            let brightness = parts.next().ok_or("No brightness value")?.parse::<u8>()?;

            return Ok(Self::new(x, y, brightness, relative, false));
        }
    }
}

// A compiled vector line using absolute coordinates.
struct VLine {
    pub x1: f32,
    pub y1: f32,
    pub x2: f32,
    pub y2: f32,
    pub brightness: u8,
    pub draw_progress: f32,
    last_fade: Instant,
}

impl VLine {
    pub fn new(x1: f32, y1: f32, x2: f32, y2: f32, brightness: u8) -> Self {
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
}

/// Max value for signed 12-bit integer.
const MAX_COORD: f32 = 4095.0;
/// Min value for signed 12-bit integer.
const MIN_COORD: f32 = -4095.0;

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

        start_window(options, None, true, fancy_mode);
    } else {
        let input_file = matches.value_of("input").expect("Input file invalid");

        println!("Opening file...");

        let mut commands_to_process = open_file(input_file).expect("Could not open file");

        println!("Opened file with {} commands", commands_to_process.len());

        //for command in commands_to_process.iter_mut() {
        //println!("{:?}", command);
        //}

        start_window(options, Some(commands_to_process), false, fancy_mode);
    }
}

fn start_window(
    options: Options,
    instructions: Option<Vec<VCommand>>,
    pipe_mode: bool,
    fancy_mode: bool,
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

    let mut drawn_lines: Vec<VLine> = Vec::new();

    let mut current_x = 0.;
    let mut current_y = 0.;
    let mut last_frame_time = Instant::now();
    let mut halted = false;
    let mut current_buffer = 0;
    let mut commands_to_run = Vec::new();
    let stdin_channel = spawn_stdin_channel();

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

                if pipe_mode {
                    match stdin_channel.try_recv() {
                        Ok(buffer) => {
                           
                            if buffer == "0\n" && current_buffer == 1 {
                                current_buffer = 0;
                                halted = false;
                            } else if buffer == "1\n" && current_buffer == 0 {
                                current_buffer = 1;
                                halted = false;
                            }

                            if !halted {
                                let dir = env::current_dir().unwrap();
                                commands_to_run = open_file(
                                    format!("buff{}", current_buffer).as_str(),
                                )
                                .expect(
                                    format!("Could not open file; Current Dir {}", dir.display())
                                        .as_str(),
                                );
                            }
                        }
                        Err(TryRecvError::Empty) => {}
                        Err(TryRecvError::Disconnected) => panic!("Channel disconnected"),
                    }
                } else {
                    commands_to_run = instructions.as_ref().unwrap().to_vec();
                }

                if !halted {
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
    
                    // Get current time
                    let now = Instant::now();
    
                    if fancy_mode {
                        // First, fade all lines that have been drawn
                        for line in drawn_lines.iter_mut() {
                            let progress = line.update_progress(
                                last_frame_time,
                                options.speed_of_motor,
                                canvas.width() as f32,
                                canvas.height() as f32,
                            );
                            if progress == 1. {
                                line.fade_if_old(now, options.fade_rate);
                            }
                            //println!("Prog: {}, Bright: {}, Dead: {}",progress,line.brightness,line.is_dead());
                        }
    
                        // Then, remove dead lines
                        //println!("All : {}",drawn_lines.len());
                        drawn_lines.retain(|line| !line.is_dead());
                        //println!("Trim: {}",drawn_lines.len());
    
                        // Draw all old lines
                        for line in drawn_lines.iter() {
                            draw_vector_line(&mut canvas, line);
                        }
    
                    }
    
                    
                    // Draw next new lines if time has elapsed
                    if last_frame_time.elapsed() > Duration::from_secs_f32(options.frame_delay)
                        || !fancy_mode
                    {
                        last_frame_time = Instant::now();
    
                        while !commands_to_run.is_empty() {
                            let new_command = commands_to_run.remove(0);
    
                            let mut line: VLine;
                            //println!("Line from ({}, {})", current_x, current_y);
    
                            if new_command.halt {
                                //println!("HALTED");
                                halted = true;
                            }
    
                            // If it's relative, then add current position to the command
                            if new_command.is_relative() {
                                line = VLine::new(
                                    current_x,
                                    current_y,
                                    current_x + new_command.x,
                                    current_y + new_command.y,
                                    new_command.brightness,
                                );
                                // Update current x/y
                                current_x = new_command.x + current_x;
                                current_y = new_command.y + current_y;
                            } else {
                                line = VLine::new(
                                    current_x,
                                    current_y,
                                    new_command.x,
                                    new_command.y,
                                    new_command.brightness,
                                );
    
                                // Update current x/y
                                current_x = new_command.x;
                                current_y = new_command.y;
                            }
    
                            if !fancy_mode {
                                line.finish();
                            }
    
                            //println!(" to ({}, {})", current_x, current_y);
                            draw_vector_line(&mut canvas, &line);
                            drawn_lines.push(line);
                        }
                    }
                    canvas.save();
                    canvas.reset();
                    canvas.restore();
                    canvas.flush();
                    windowed_context.swap_buffers().unwrap();
                }
            }
            Event::MainEventsCleared => windowed_context.window().request_redraw(),
            _ => (),
        }
    });
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
    let mut r = |x1: f32, y1: f32, x2: f32, y2: f32, brightness: u8| {
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
            brightness as f32 / 500.,
            brightness as f32 / 255.,
            brightness as f32 / 255.,
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
