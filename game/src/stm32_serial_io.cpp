#include "../include/rawio.h"
#include "stm32f4xx.h"

#define TIM TIM2
#define USART USART2
#define DMA_STREAM DMA1_Stream6

// The size of buffer to use before sending data to the vector generator
// Larger buffers will take longer to transmit, but also will result in fewer
// pauses (it needs to pause while sending data if you try to send more)
#define BUFFER_SIZE 128
static uint8_t output_buffer[BUFFER_SIZE];
static uint32_t output_buffer_size;

InputState get_inputs() {
    InputState state = { .xpos=0, .ypos=0, .buttons=0};
    // TODO retreive the data from the joystick and/or buttons
    return state;
}

void initialize_input_output() {
    // Initialize global variables
    output_buffer_size = 0;
    // Enable peripherals: GPIOA, DMA, TIM2, UART2
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_DMA1EN | RCC_APB1ENR_TIM2EN | RCC_APB1ENR_USART2EN;
    // Configure GPIO pin modes as alternate function 7 (has USART controls)
    GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk);
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODER2_Pos);
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
    GPIOA->AFR[0] |= (0b0111 << GPIO_AFRL_AFSEL2_Pos | 0b0111 << GPIO_AFRL_AFSEL3_Pos);

    // Configure USART2 
    USART->CR1 |= (USART_CR1_UE_Msk); // Enable USART
    USART->CR1 &= ~(USART_CR1_M_Msk); // M=0 corresponds to 8 data bits
    USART->CR2 &= ~(USART_CR2_STOP_Msk); // 0b00 corresponds to 1 stop bit
    USART->CR1 &= ~(USART_CR1_OVER8_Msk); // Set to 16 times sampling freq

    USART->CR3 |= (USART_CR3_DMAT_Msk);
    USART->BRR |= (8 << USART_BRR_DIV_Mantissa_Pos);
    USART->BRR |= (11 << USART_BRR_DIV_Fraction_Pos); // 11/16

    USART->CR1 |= (USART_CR1_TE_Msk); // Enable USART2

    // DMA1 configuration (channel 3 / stream 5) for USART2
    // SxCR register:
    // - Memory-to-peripheral
    // - Increment memory ptr, don't increment periph ptr.
    // - 8-bit data size for both source and destination.
    // - High priority (2/3).

    // Reset DMA1 Stream 6
    DMA_STREAM->CR &= ~( DMA_SxCR_CHSEL |
                            DMA_SxCR_PL |
                            DMA_SxCR_MSIZE |
                            DMA_SxCR_PSIZE |
                            DMA_SxCR_PINC |
                            DMA_SxCR_EN );

    // Set up DMA1 Stream 6
    DMA_STREAM->CR |= ((0b100 << DMA_SxCR_CHSEL_Pos) |
                       (0b00 << DMA_SxCR_MSIZE_Pos) |
                       (0b00 << DMA_SxCR_PSIZE_Pos) |
                       (0b10 << DMA_SxCR_PL_Pos) |
                       DMA_SxCR_MINC |
                       (0b1 << DMA_SxCR_DIR_Pos));
    

    // Set DMA source and destination addresses.
    // Source: Address of the buffer in memory.
    DMA_STREAM->M0AR = (uint32_t) &output_buffer;
    // Dest.: USART data register
    DMA_STREAM->PAR  = (uint32_t) &(USART->DR);

    // Configure TIM2
    // Set one pulse mode
    TIM->CR1 |= TIM_CR1_OPM;
}

// Wait for any transmission in progress to end
static inline void wait_for_buffer_send() {
    // TODO use interrupts instead
    while ((!USART->SR) & USART_SR_TXE_Msk) ;
}
static inline void send_data() {
    if (output_buffer_size == 0) {
        return;
    }
    wait_for_buffer_send();
    // Clear USART TC flag
    USART->SR &= ~(USART_SR_TC_Msk);
    // Set DMA source address to the address of the buffer in memory.
    DMA_STREAM->M0AR = (uint32_t) &output_buffer;
    // Set DMA data transfer length (# of samples).
    DMA_STREAM->NDTR = (uint32_t) output_buffer_size;
    // Enable DMA stream.
    DMA_STREAM->CR |= DMA_SxCR_EN;
    output_buffer_size = 0;
}
// Send data if the buffer is nearly full
static inline void decide_send_data() {
    if (output_buffer_size > BUFFER_SIZE - 5) {
        send_data();
    }
}
static inline void queue_byte(uint8_t byte) {
    wait_for_buffer_send();
    output_buffer[output_buffer_size] = byte;
    output_buffer_size += 1;
    decide_send_data();
}
static inline void queue_quad_byte(uint32_t bytes) {
    wait_for_buffer_send();
    output_buffer[output_buffer_size] = (uint8_t) bytes >> 24;
    output_buffer[output_buffer_size+1] = (uint8_t) bytes >> 16;
    output_buffer[output_buffer_size+2] = (uint8_t) bytes >> 8;
    output_buffer[output_buffer_size+3] = (uint8_t) bytes;
    output_buffer_size += 4;
    decide_send_data();
}

void draw_buffer_switch() {
    queue_byte(0x7);
    send_data();
}
void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness) {
    queue_quad_byte(
        (0x3 << 30) | ((delta_x & ((1 << 10)-1)) << 20) | ((delta_y & ((1 << 10)-1)) << 10) | (brightness & ((1 << 10)-1))
    );
}
void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness) {
    queue_quad_byte(
        (0x2 << 30) | ((x_position & ((1 << 10)-1)) << 20) | ((y_position & ((1 << 10)-1)) << 10) | (brightness & ((1 << 10)-1))
    );
}
void draw_end_buffer() {
    queue_byte(0);
}

void set_sleep_time_ms(uint32_t milliseconds) {
    // TODO correct this implement this
    // TIM2 configuration.
    // Set prescaler and autoreload to issue DMA request at 1 Hz
    TIM->PSC  =  0x0000;
    TIM->ARR  =  SystemCoreClock;
    
    // Enable trigger output on timer update events.
    TIM->CR2 &= ~(TIM_CR2_MMS);
    TIM->CR2 |=  (0x2 << TIM_CR2_MMS_Pos);
    
    // Configure interrupt enable on update event
    NVIC_EnableIRQ(TIM2_IRQn);
}
static volatile int sleeping=0;
static void sleep_timer_interrupt() {
    sleeping = 1;
}
bool sleep_until_set_time() {
    // TODO check that the timer isn't going off in the past
    sleeping = 0;
    TIM->CR1 |= TIM_CR1_CEN;
    while (!sleeping) {}
    return true;
}
