#include "../include/rawio.h"
#include "stm32f4xx.h"

InputState get_inputs() {
    InputState state = { .xpos=0, .ypos=0, .buttons=0};
    // TODO retreive the data from the joystick and/or buttons
    return state;
}

void initialize_input_output() {
    // TODO any needed initialization
    // (probably turning on uart to vector generator)
}

static inline void send_bit(uint8_t bit) {
    // TODO send the bit over UART
}
static inline void send_bits(uint32_t bits, uint8_t length) {
    while (length > 0) {
        send_bit((bits & (1 << (length-1))) >> (length-1));
        length--;
    }
}

void draw_buffer_switch() {
    send_bits(0x7, 8);
}
void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness) {
    send_bits(
        (0x3 << 30) | ((delta_x & ((1 << 10)-1)) << 20) | ((delta_y & ((1 << 10)-1)) << 10) | (brightness & ((1 << 10)-1)),
        32
    );
}
void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness) {
    send_bits(
        (0x2 << 30) | ((x_position & ((1 << 10)-1)) << 20) | ((y_position & ((1 << 10)-1)) << 10) | (brightness & ((1 << 10)-1)),
        32
    );
}
void draw_end_buffer() {
    send_bits(0, 8);
}

void set_sleep_time_ms(uint32_t milliseconds) {
    // TODO actually implement this
}
bool sleep_until_set_time() {
    // TODO actually implement this
    while (1);
}
