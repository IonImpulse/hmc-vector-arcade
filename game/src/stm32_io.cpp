// stm32_io.h
// Implements rawio.h on the STM32F401RE
// This file focuses on the algorithmic side of implementing inputs, sound generation, and vector generation
// See stm32_helper.h for details on how peripherals are driven
#include "../include/rawio.h"
#include "stm32f4xx.h"
#include "stm32_helper.h"
#include "gpio.h"

// Vector Bitfields
#define NEG (1<<10)    // applies negative direction; for use with x or y data
#define BLANK (1<<0)   // blanks current vector (i.e. makes it transparent); for use with z data
#define LD_COL (1<<1)  // loads new color data; for use with z data
#define LD_POS (1<<2)  // loads new absolute position; for use with z data

// Vector Buffers
#define BUFFER_SIZE 200
size_t readPtr;
size_t writePtr;
typedef struct {
    uint16_t x[BUFFER_SIZE];
    uint16_t y[BUFFER_SIZE];
    uint16_t z[BUFFER_SIZE];
    size_t length;
} vector_buffer;
vector_buffer buff0;
vector_buffer buff1;
vector_buffer* readBuff = &buff0;
vector_buffer* writeBuff = &buff1;

// Current Position Being Written to Buffer
uint16_t abs_x=0;
uint16_t abs_y=0;

// Current Instruction Being Drawn to Screen
uint16_t curr_x=0;
uint16_t curr_y=0;
uint16_t curr_z=0;
int curr_shift=0;

bool halt_requested=0; // game program raises halt_requested when it wants to swap buffers
bool halted=1; // vector generator raises halted to signify that it is ready to swap buffers

InputState get_inputs() {
    InputState state = { .xpos=0, .ypos=0, .buttons=0};
    // TODO retreive the data from the joystick and/or buttons
    return state;
}

void initialize_input_output() {
    initalize_embedded_system();
}

void get_next_vector() {
    // Read in next vector
    curr_x = readBuff->x[readPtr];
    curr_y = readBuff->y[readPtr];
    curr_z = readBuff->z[readPtr];
    readPtr++;

    // Binary Normalization Step
    uint16_t mag_x = curr_x & 0x1FF;
    uint16_t mag_y = curr_y & 0x1FF;
    curr_shift = 0;
    while (((mag_x & 0x100)==0)&&((mag_y & 0x100)==0)&&(curr_shift<7)) {
        mag_x <<= 1;
        mag_y <<= 1;
        curr_shift += 1;
    }

    // Send out the next vector over SPI
    SPIsend(X_SPI,(curr_x&0xFE00)|mag_x);
    SPIsend(Y_SPI,(curr_y&0xFE00)|mag_y);
}

void draw_buffer_switch() {
    if (readBuff==&buff0) {
        readBuff  = &buff1;
        writeBuff = &buff0;
    } else {
        readBuff  = &buff0;
        writeBuff = &buff1;
    }
    readPtr=0;
    writePtr=0;
    halted=0;
    halt_requested=0;
    // load in initial vector to get things started
    get_next_vector();
    // now generate an update to start the interrupt cycle
    generateDuration(VEC_TIMER, 20, 20);
}

void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness) {
    uint16_t signMagX = (delta_x>=0) ? (uint16_t)(delta_x) : NEG|(uint16_t)(-delta_x);
    uint16_t signMagY = (delta_y>=0) ? (uint16_t)(delta_y) : NEG|(uint16_t)(-delta_y);
    writeBuff->x[writePtr] = signMagX;//((brightness>>4)<<12) | signMagX;
    writeBuff->y[writePtr] = signMagY;//((brightness&0xF)<<12) | signMagY;
    writeBuff->z[writePtr] = 0;
    writePtr++;
    abs_x += delta_x;
    abs_y += delta_y;
}

void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness) {
    int16_t delta_x = x_position - abs_x;
    int16_t delta_y = y_position - abs_y;
    draw_relative_vector(delta_x, delta_y, brightness);
}

void draw_end_buffer() {
    // absolute load counters to center of screen
    writeBuff->x[writePtr] = 512;
    writeBuff->y[writePtr] = 512;
    writeBuff->z[writePtr] = LD_POS | BLANK;
    writePtr++;
    writeBuff->length = writePtr;
}

bool is_halted() {return halted;}
void request_halt() {halt_requested=1;}

// This is the main function for handling drawing stuff
extern "C" void TIM5_IRQHandler() {
    //sendString("cheese");
    // Clear interrupt flag
    VEC_TIMER->SR &= ~(TIM_SR_UIF);
    if (halted) return;
    // Output the previous vector's data by strobing shift reg latch
    digitalWrite(X_SHIFT_REG_LD_GPIO, X_SHIFT_REG_LD_PIN, GPIO_HIGH);
    digitalWrite(Y_SHIFT_REG_LD_GPIO, Y_SHIFT_REG_LD_PIN, GPIO_HIGH);
    digitalWrite(X_SHIFT_REG_LD_GPIO, X_SHIFT_REG_LD_PIN, GPIO_LOW);
    digitalWrite(Y_SHIFT_REG_LD_GPIO, Y_SHIFT_REG_LD_PIN, GPIO_LOW);
    // Strobe color latch if we need to
    if (curr_z&LD_COL) {
        digitalWrite(COLOR_LD_GPIO, COLOR_LD_PIN, GPIO_HIGH);
        digitalWrite(COLOR_LD_GPIO, COLOR_LD_PIN, GPIO_LOW);
    }
    // Strobe counter parallel load if we need to 
    // This moves the beam to absolute position (X,Y)
    if (curr_z & LD_POS) {
        // Blank colors for absolute loads
        digitalWrite(BLANKb_GPIO, BLANKb_PIN, GPIO_LOW);
        // Strobe the counter parallel load
        digitalWrite(COUNT_LDb_GPIO, COUNT_LDb_PIN, GPIO_LOW);
        digitalWrite(COUNT_LDb_GPIO, COUNT_LDb_PIN, GPIO_HIGH);
        
        // Give some time for the beam to settle
        // but set compare value CCR1 so that GOb never activates and accidentally draws a vector
        generateDuration(VEC_TIMER, 1028, 1028);
    } else {
        // Blank colors if we need to
        if (curr_z&BLANK) {
            digitalWrite(BLANKb_GPIO,BLANKb_PIN,GPIO_LOW);
        } else {
            digitalWrite(BLANKb_GPIO,BLANKb_PIN,GPIO_HIGH);
        }
        // Activate GOb for 1024 pulses
        // This runs the BRM's and Up/Down counters so that they draw out a line
        // The compare functionality needs a few cycles to get ready after the timer is enabled,
        // hence the offset by 5 ticks.
        generateDuration(VEC_TIMER, (1024>>curr_shift)+5, 5);
    }

    get_next_vector();

    // Was this the last vector?
    if (readPtr > readBuff->length) {
        if (halt_requested) {
            // Halt if the main code is saying it wants to swap buffers
            halted=1;
        } else {
            // Otherwise redraw the current frame
            readPtr=0;
        }
    }
}

void start_timer(uint32_t milliseconds) {
    start_micros(DELAY_TIM, milliseconds*1000);
}
bool timer_done() {
    return tim_done(DELAY_TIM);
}
void printChar(char data) {
    USART2->DR = data;
}
void sendString(const char* txStr) {
    sendString_USART2(txStr);
}