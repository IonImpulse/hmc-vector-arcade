#ifndef RAWIO_H
#define RAWIO_H

#include <cstdint>
#include <memory>

struct InputState {
    // X position of joystick, from -1 to 1
    float xpos;
    // Y position of joystick, from -1 to 1
    float ypos;
    // Bitmap of which buttons are pressed
    uint32_t buttons;
};

InputState get_inputs();

void initialize_input_output();

void draw_buffer_switch();
void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness);
void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness);
void draw_end_buffer();
void request_halt();
bool is_halted();

void start_timer(uint32_t milliseconds);
bool timer_done();

void printChar(char data);
void sendString(const char* txStr);
#endif
