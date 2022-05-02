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
    char z[BUFFER_SIZE];
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

bool halted=1; // vector generator raises halted to signify that it is ready to swap buffers
int halt_state=0;

#define DEBUG_OUTPUT 1

void refresh_inputs() {
    // this function starts the ADC conversions so that they are probably finished by the time get_inputs() is called
    adc_start_conversion();
}

InputState get_inputs() {
    InputState state = { .xpos=0, .ypos=0, .buttons=0};
    state.xpos = ((float)(read_adc())) / 4096.0;
    state.ypos = ((float)(read_adc())) / 4096.0;

    // TODO retreive the data from the buttons (if we have any)

    #if DEBUG_OUTPUT
    char data[256];
        snprintf(data, 256, "x: %d.%02d, y: %d.%02d\n",
            (int)state.xpos, ((int)(state.xpos * 100)) % 100,
            (int)state.ypos, ((int)(state.ypos * 100)) % 100);
        sendString(data);
    #endif

    return state;
}

void initialize_input_output() {
    initalize_embedded_system();
}

void get_next_vector() {
    // Read in next vector
    if (halted) {
        // If we're halted, we need to draw invisible garbage
        // to prevent the spot killer circuit from thinking our signal is dead
        switch (halt_state) {
        case 0:
            curr_x = 612;
            curr_y = 612;
            curr_z = LD_POS | BLANK;
            halt_state=1;
            break;
        case 1:
            curr_x = NEG|200;
            curr_y = 0;
            curr_z = BLANK;
            halt_state=2;
            break;
        case 2:
            curr_x = 0;
            curr_y = NEG|200;
            curr_z = BLANK;
            halt_state=3;
            break;
        case 3:
            curr_x = 200;
            curr_y = 0;
            curr_z = BLANK;
            halt_state=4;
            break;
        case 4:
            curr_x = 0;
            curr_y = 200;
            curr_z = BLANK;
            halt_state=1;
            break;
        }
    } else {
        curr_x = readBuff->x[readPtr];
        curr_y = readBuff->y[readPtr];
        curr_z = readBuff->z[readPtr];
        readPtr++;
    }
    if (curr_z&LD_POS) {
        // Load Absolute Position
        SPIsend(X_SPI,curr_x);
        SPIsend(Y_SPI,curr_y);
    } else {
        // Draw Vector
        //
        // Binary Normalization Step
        //   If the distance integer can be doubled without affecting the sign bit,
        //   do so, and halve the number of clock pulses.
        //   This ensures the drawing speed is about the same for both long and short vectors.
        uint16_t mag_x = curr_x & 0x3FF;
        uint16_t mag_y = curr_y & 0x3FF;
        curr_shift = 0;
        while ((mag_x<0x1FF)&&(mag_y<0x1FF)&&(curr_shift<9)) {
            mag_x <<= 1;
            mag_y <<= 1;
            curr_shift += 1;
        }

        // Send out the next vector over SPI
        SPIsend(X_SPI,(curr_x&0xFC00)|mag_x);
        SPIsend(Y_SPI,(curr_y&0xFC00)|mag_y);
    }
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
    load_abs_pos(0,0);
    load_abs_pos(0,0);
    get_next_vector();
    // now generate an update to start the interrupt cycle
    generateDuration(VEC_TIMER, 20, 20);
}

void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness) {
    uint16_t signMagX = (delta_x>=0) ? (uint16_t)(delta_x) : NEG|(uint16_t)(-delta_x);
    uint16_t signMagY = (delta_y>=0) ? (uint16_t)(delta_y) : NEG|(uint16_t)(-delta_y);
    writeBuff->x[writePtr] = ((brightness>>4)<<12) | signMagX;
    writeBuff->y[writePtr] = signMagY;
    writeBuff->z[writePtr] = 0;
    writePtr++;
    abs_x += delta_x;
    abs_y += delta_y;
}

void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness) {
    int16_t delta_x = x_position - abs_x;
    int16_t delta_y = y_position - abs_y;
    draw_relative_vector(delta_x, delta_y, brightness);
    abs_x=x_position;
    abs_y=y_position;
}

void load_abs_pos(int16_t x_position, int16_t y_position) {
    draw_absolute_vector(x_position, y_position, 0);
    writeBuff->x[writePtr] = (uint16_t)(x_position+512);
    writeBuff->y[writePtr] = (uint16_t)(y_position+512);
    writeBuff->z[writePtr] = LD_POS | BLANK;
    writePtr++;
}

void draw_end_buffer() {
    load_abs_pos(0,0);
    writeBuff->length = writePtr;
}

bool is_halted() {return halted;}

// This is the main function for handling drawing stuff
extern "C" void TIM5_IRQHandler() {
    // Clear interrupt flag
    VEC_TIMER->SR &= ~(TIM_SR_UIF);
    // Make sure shift register has time to finish
    for (volatile int i=0;i<10;i++) {}
    // Output the previous vector's data by strobing shift reg latch
    digitalWrite(X_SHIFT_REG_LD_GPIO, X_SHIFT_REG_LD_PIN, GPIO_HIGH);
    digitalWrite(Y_SHIFT_REG_LD_GPIO, Y_SHIFT_REG_LD_PIN, GPIO_HIGH);
    digitalWrite(X_SHIFT_REG_LD_GPIO, X_SHIFT_REG_LD_PIN, GPIO_LOW);
    digitalWrite(Y_SHIFT_REG_LD_GPIO, Y_SHIFT_REG_LD_PIN, GPIO_LOW);
    // Strobe color latch if we need to
    if (curr_z&LD_COL) {
        // Hmm latch doesn't seem to like this...
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
        generateDuration(VEC_TIMER, 100, 100);
    } else {
        // Blank colors if we need to
        if (curr_z&BLANK) {
            digitalWrite(BLANKb_GPIO,BLANKb_PIN,GPIO_LOW);
        } else {
            digitalWrite(BLANKb_GPIO,BLANKb_PIN,GPIO_HIGH);
        }
        // Activate GOb for 1024 / normalization_factor to draw out vector
        // The compare functionality needs a few cycles to get ready after the timer is enabled,
        // hence the offset by 5 ticks.
        generateDuration(VEC_TIMER, (1024>>curr_shift)+5, 5);
    }

    get_next_vector();

    // Was this the last vector?
    if (readPtr > readBuff->length) {
        // Show that vector generator has completed frame
        if (halted==0) {
            halt_state=0;
        }
        halted=1;
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

void requestChiptune(chiptuneType type, int soundID) {
    // Bit [7] background music or foreground sound effect
    // Bits [6:1] sound ID
    // Bit [0] always 1 for hardware protocol reasons
    char byte = (type << 7) | ((0x3f & soundID) << 1) | 1;
    SPIsend(SOUND_SPI,byte);
}
