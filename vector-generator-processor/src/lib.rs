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

#[no_mangle]
extern "C" fn main() -> i32 {
    // For now, we just blink an LED
    let gpio = unsafe { (GPIO_ADDRESS as *mut GpioPin).as_mut() }.unwrap();
    gpio.output_en |= 1 << 5;
    loop {
        gpio.output_val ^= 1 << 5;
    }
}
