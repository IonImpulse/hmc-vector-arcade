#ifndef RAWIO_H
#define RAWIO_H

#include <cstdint>
#include <memory>

struct InputState {
    // joystick position from -1 to 1 in each axis
    float joyX, joyY;
    // bitmap of which buttons are pressed
    uint32_t buttons;
    uint32_t pressedButtons; // only high for the frame where button transitions from unpressed to pressed
    // mouse state (only relevant for simulated IO)
    int mouseX, mouseY;
    uint32_t mouseButtons;
    uint32_t pressedMouseButtons;
    int mouseMoved;
    int mouseScrollY;
    // keyboard state (only relevant for simulated IO)
    int numkeys = -1;
    const uint8_t* keys;
    uint8_t prevKeys[1000]; // guestimate that there won't be more than 1000 keys on the keyboard (512 found in reality)
};
InputState get_inputs();

void initialize_input_output();

void draw_buffer_switch();
void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness);
void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness);
void load_abs_pos(int16_t x_position, int16_t y_position);
void draw_end_buffer();
bool is_halted();

void start_timer(uint32_t milliseconds);
void start_frame_timer();
bool timer_done();

void printChar(char data);
void sendString(const char* txStr);

enum chiptuneType{song=1,sfx=0};
void requestChiptune(chiptuneType type, int soundID);
#endif
