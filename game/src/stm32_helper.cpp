#include "stm32_helper.h"
#include "gpio.h"
#include "string.h"

//////////////////////////////////////
// System Clock Speed Configuration //
//////////////////////////////////////
void configureFlash() {
    // This function is needed because the default flash configuration breaks at high clock speed

    // Set to 2 waitstates
    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

    // Turn on the ART
    FLASH->ACR |= FLASH_ACR_PRFTEN;
}

void configure84MHzPLL() {
    /*
     Set clock to 84 MHz
     Output freq = (src_clk) * (N/M) / P
          84 MHz = (8 MHz) * (336/8) / 4
     M:8, N:336, P:4
     Use HSE as src_clk; on the Nucleo, it is connected to 8 MHz ST-Link clock
    */

    RCC->CR &= ~(RCC_CR_PLLON); // Turn off PLL
    RCC->CR &= ~(RCC_CR_PLLI2SON); // Turn off the I2S PLL too (it shares M and src_clk)
    while (RCC->CR & RCC_CR_PLLRDY); // Wait till PLL is unlocked (e.g., off)

    // Select HSE as src_clk
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

    // Set M
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
    RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLM_Pos);

    // Set N
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
    RCC->PLLCFGR |= (336 << RCC_PLLCFGR_PLLN_Pos);

    // Set P (yes, 0b01 is interpreted to mean a factor of 4)
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
    RCC->PLLCFGR |= (0b01 << RCC_PLLCFGR_PLLP_Pos);

    RCC->CR |= RCC_CR_PLLON; // Turn on PLL
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Wait till PLL is locked
}

void configure84MHzClock() {
    /* Sets system clock to 84 MHz from the PLL which is fed 8 MHz from HSE */

    configureFlash(); // configure flash to support the higher clock speed

    // Turn on and bypass for HSE from ST-LINK
    RCC->CR |= RCC_CR_HSEBYP;
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));

    // Configure and turn on PLL
    // Note that this will have the side effects of turning off the I2S PLL
    // and possibly changing its input configuration.
    configure84MHzPLL();

    // Select PLL as clock source
    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

    // Set AHB (system clock) prescalar to 0 so we get full speed!
    RCC->CFGR &= ~(RCC_CFGR_HPRE);
    // Set APB2 (high-speed bus) prescaler to no division
    // (this will let our clocks receive the full SYSCLK freq)
    RCC->CFGR &= ~(RCC_CFGR_PPRE2);
    // Set APB1 (low-speed bus) to divide by 2 (because APB1 should not exceed 42 MHz)
    RCC->CFGR &= ~(RCC_CFGR_PPRE1);
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
    // Note that clocks on APB1 will still get full 84 MHz if APB1 at 42 MHz
}










/////////
// SPI //
/////////
// for the vector X and Y axes
void configureSPI(SPI_TypeDef * SPIx) {
    SPIx->CR1 &= ~(SPI_CR1_SPE); // Disable SPI so we can change all the settings
                                 // I assume user calls this on startup, but if you want to change
                                 // midway, there may be more complicated shutdown procedures
    // Baud Rate: divide by 4
    SPIx->CR1 &= ~(SPI_CR1_BR);
    SPIx->CR1 |= (0b001 << SPI_CR1_BR_Pos);

    SPIx->CR1 &= ~(SPI_CR1_CPOL); // Set polarity to idle low
    SPIx->CR1 &= ~(SPI_CR1_CPHA); // Set phase to leading edge

    SPIx->CR1 |= SPI_CR1_DFF; // 16-bit data
    SPIx->CR1 |= SPI_CR1_LSBFIRST; // LSB first

    SPIx->CR1 &= ~(SPI_CR1_BIDIMODE); // For full-duplex, we use both wires, and each is unidirectional
    SPIx->CR1 &= ~(SPI_CR1_RXONLY); // For full-duplex, we don't want receive only
    SPIx->CR1 |= SPI_CR1_MSTR; // Make STM32 act as the master
    SPIx->CR1 |= SPI_CR1_SSM; // To give software control of nSS...
    SPIx->CR1 |= SPI_CR1_SSI; // ...and set internal nSS. You could also set SSOE instead.
    SPIx->CR1 &= ~(SPI_CR1_CRCEN); // Disable CRC calculation by default; if you need it, write your own lib!
    SPIx->CR1 |= SPI_CR1_SPE; // Enable SPI
}
// for the communicating with chiptune CPU
void configureSoundSPI(SPI_TypeDef * SPIx) {
    SPIx->CR1 &= ~(SPI_CR1_SPE); // Disable SPI so we can change all the settings
                                 // I assume user calls this on startup, but if you want to change
                                 // midway, there may be more complicated shutdown procedures
    // Baud Rate: divide by 8
    SPIx->CR1 &= ~(SPI_CR1_BR);
    SPIx->CR1 |= (0b100 << SPI_CR1_BR_Pos);

    SPIx->CR1 &= ~SPI_CR1_DFF; // 8-bit data

    SPIx->CR1 &= ~(SPI_CR1_BIDIMODE);
    SPIx->CR1 |= SPI_CR1_BIDIOE;
    SPIx->CR1 &= ~(SPI_CR1_RXONLY);
    SPIx->CR1 |= SPI_CR1_MSTR; // Make STM32 act as the master
    SPIx->CR1 &= ~SPI_CR1_SSM; // To give hardware control of nSS
    //SPIx->CR2 &= ~SPI_CR2_SSOE; // needed to make nSS go automatically?
    SPIx->CR2 |= SPI_CR2_FRF; // TI mode -- for some reason this is needed to make nSS go automatically
    SPIx->CR1 &= ~(SPI_CR1_CRCEN); // Disable CRC calculation by default
    SPIx->CR1 |= SPI_CR1_SPE; // Enable SPI
}

void SPIsend(SPI_TypeDef * SPIx, uint16_t data) {
    // Transmit
    while (!(SPIx->SR & SPI_SR_TXE)); // Wait until TX buffer is ready for data to be written
    SPIx->DR = data; // load data into TX buffer to begin transfer
}










////////////
// Timers //
////////////
void configureTimer(TIM_TypeDef * TIMx) {
    // Set prescaler division factor
    TIMx->PSC = (uint32_t)(84-1); // Assuming 84 MHz
    // Generate an update event to update prescaler value
    TIMx->EGR |= TIM_EGR_UG;
    // Enable counter
    TIMx->CR1 |= TIM_CR1_CEN;
}

void configureCaptureCompare(TIM_TypeDef * TIMx) {
    /* Disable Counter */
    TIMx->CR1 &= ~(TIM_CR1_CEN);                    // disable counter
    /* Configure Capture/Compare Channel 1 for PWM Output */
    TIMx->CCER &= ~(TIM_CCER_CC1E);                 // disable capture/compare channel 1 so we can fiddle with its settings
    TIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);               // capture/compare channel to output mode
    TIMx->CCMR1 |= (0b111 << TIM_CCMR1_OC1M_Pos);   // set capture/compare channel 1 to PWM mode
    TIMx->CCER |= TIM_CCER_CC1P;                    // set polarity to active low
    TIMx->CCMR1 |= TIM_CCMR1_OC1PE;                 // set capture/compare channel 1 preload enable
    TIMx->CCER |= TIM_CCER_CC1E;                    // enable capture/compare channel 1
}

void configureDuration(TIM_TypeDef * TIMx, uint32_t prescale, uint8_t slave_mode, uint8_t master_src) {
    /* Configures a timer for outputting large precise delays.
       Given a TIMER_BASE_FREQ of 84 MHz, the resolution (each counter tick) is 1 / 84 MHz * prescale
       Likewise the longest countable duration is resolution * 2^(bits in ARR)
    */

    /* Disable Counter */
    TIMx->CR1 &= ~(TIM_CR1_CEN); // disable counter
    /* Select input clock */
    if (slave_mode) {
        // select which master's trigger to respond to (see table 54 of ref manual)
        TIMx->SMCR &= ~(TIM_SMCR_TS);
        TIMx->SMCR |= (master_src << TIM_SMCR_TS_Pos);
        // select external clock mode (aka output of the trigger selection mux)
        TIMx->SMCR |= (0b111 << TIM_SMCR_SMS_Pos);
    } else {
        // disable slave mode controller;
        // use instead internal clock CK_INT (aka "TIMx_CLK from RCC")
        TIMx->SMCR &= ~(TIM_SMCR_SMS);
    }

    /* Prescale */
    TIMx->PSC = prescale;

    /* Configure Counter as Upcounter */
    TIMx->CR1 &= ~(TIM_CR1_CMS); // use edge-aligned mode (i.e. plain vanilla up or down counting)
    TIMx->CR1 &= ~(TIM_CR1_DIR); // upcounter mode

    /* Allow Automatic Updating and Interrupting on Overflow Events */
    TIMx->CR1 |= TIM_CR1_ARPE;  // "auto-reload preload enabled"
                                // transfers preload register to shadow register every update event, meaning
                                // we can change ARR and PSC without having to manually request an update
    TIMx->ARR = 1;  // set max count to 1 as a default so that
                    // we generate update events quickly when we turn timer on
    TIMx->CR1 |= TIM_CR1_OPM;   // stop counter at update events; we want to time out just one pulse
    //TIMx->DIER |= TIM_DIER_UIE; // enable interrupts upon updating

    /* Implement Settings */
    TIMx->CR1 |= TIM_CR1_URS;   // let only counter under/overflows generate interrupts
                                // so that when we manually generate an update, it doesn't make an interrupt
    TIMx->EGR |= TIM_EGR_UG;    // manually generate an update to initialize all shadow registers
}

void generateDuration(TIM_TypeDef * TIMx, uint32_t duration, uint32_t compare_val) {
    /* Generates an update after the specified duration.
     * The actual time duration is resolution (determined by configureDuration) * duration
     * Note that duration should not exceed 2^31-1 (TIM2,5) or 2^15-1 (TIM3,4) */
    TIMx->CR1 |= TIM_CR1_UDIS;  // Disable update events because apparently it would be bad
                                // if the timer by chance tried to update the shadow registers
                                // while we're writing new values in the preload registers.
    TIMx->ARR = duration;
    TIMx->CCR1 = compare_val;

    TIMx->CR1 &= ~(TIM_CR1_UDIS);   // enable update events;
                                    // (i.e. send update signal every time counter under/over-flows)
                                    // In duration mode, this will generate an interrupt.

    TIMx->EGR |= TIM_EGR_UG;  // manually generate an update to initialize all shadow registers
    TIMx->CR1 |= TIM_CR1_CEN; // enable counter
}

void configurePWM(TIM_TypeDef * TIMx, uint8_t master_mode) {
    /* Configures a timer for outputting musical tones (or more generally, PWM stuff)*/

    /* Disable Counter */
    TIMx->CR1 &= ~(TIM_CR1_CEN); // disable counter

    /* Select internal clock at maximum frequency */
    TIMx->SMCR &= ~(TIM_SMCR_SMS);  // disable slave mode controller;
                                    // use instead internal clock CK_INT (aka "TIMx_CLK from RCC")
    TIMx->PSC = 0; // do not prescale; let the counter receive full CK_INT frequency

    /* Configure Counter as Upcounter */
    TIMx->CR1 &= ~(TIM_CR1_CMS); // use edge-aligned mode (i.e. plain vanilla up or down counting)
    TIMx->CR1 &= ~(TIM_CR1_DIR); // upcounter mode

    /* Allow Automatic Updating on Overflow Events */
    TIMx->CR1 |= TIM_CR1_ARPE;  // "auto-reload preload enabled"
                                // transfers preload register to shadow register every update event, meaning
                                // we can change ARR and PSC without having to manually request an update
    TIMx->ARR = 1;  // set max count to 1 as a default so that
                    // we generate update events quickly when we turn timer on
    TIMx->CR1 &= ~(TIM_CR1_OPM); // do not stop counter at update events

    /* If Master, output trigger signal upon update events */
    if (master_mode) {
        // set update event at as trigger output (TRGO)
        TIMx->CR2 &= ~(TIM_CR2_MMS);
        TIMx->CR2 |= (0b010 << TIM_CR2_MMS_Pos);
    }

    /* Start Running */
    TIMx->EGR |= TIM_EGR_UG;  // manually generate an update to initialize all shadow registers
    TIMx->CR1 |= TIM_CR1_CEN; // enable counter
}

void generatePWMfreq(TIM_TypeDef * TIMx, uint32_t freq, uint32_t duty_inv) {
    /* Generate a signal of frequency <freq> Hz and 1/<duty_inv> duty cycle
     * This is designed for applications where frequency matters most (e.g. tone generation).
     *
     * <timer> TIM2, TIM3, TIM4, or TIM5
     * <freq> in Hz, can range from TIMER_BASE_FREQ / (ARR size) to TIMER_BASE_FREQ
     *               though it should be noted that division rounding error approaches
     *               a maximum of ~50% as freq approaches TIMER_BASE_FREQ
     * <duty_inv> is 1/duty, and it is also subjected to divison rounding error
     *               as duty approaches TIMER_BASE_FREQ / freq
     *
     * TIM2 and TIM5 have 32 bit ARR, which at 84MHz base freq, will not generate freqs below 0.196Hz
     * TIM3 and TIM4 have 16 bit ARR, which at 84MHz base freq, will not generate freqs below 1282Hz
     */

    TIMx->CR1 |= TIM_CR1_UDIS;  // Disable update events because apparently it would be bad
                                // if the timer by chance tried to update the shadow registers
                                // while we're writing new values in the preload registers.
    if (freq != 0) {
        uint32_t num_ticks = 84000000UL/freq;
                                 // Both frequencies are in Hz, so the ratio
                                 // represents number of periods of base freq per period of tone freq.
                                 // Integer division shouldn't plague us with too much rounding error
                                 // if TIMER_BASE_FREQ is sufficiently large relative to max freq,

        TIMx->ARR  = num_ticks;          // Set value we count up to.
        TIMx->CCR1 = num_ticks/duty_inv; // Set the count threshold at which capture/compare channel 1 output is high/low.
                                         // Dividing by duty_inv is what implements the duty cycle
    } else {
        TIMx->ARR = 1;  // set to 1 to that it's constantly updating
        TIMx->CCR1 = 0; // set output to resting state
    }

    TIMx->CR1 &= ~(TIM_CR1_UDIS); // enable update events;
                                  // (i.e. send update signal every time counter under/over-flows)
}

void start_micros(TIM_TypeDef * TIMx, uint32_t us) {
    TIMx->ARR = us;               // Set timer max count
    TIMx->EGR |= TIM_EGR_UG;      // Force update
    TIMx->SR &= ~(TIM_SR_UIF);    // Clear UIF
    TIMx->CNT = 0;                // Reset count
}

bool tim_done(TIM_TypeDef * TIMx) {
    return (TIMx->SR & TIM_SR_UIF)==TIM_SR_UIF;
}










////////////////////
// USART2 and DMA //
////////////////////
void configureUSART2() {
    USART2->CR1 |= (USART_CR1_UE_Msk); // Enable USART
    USART2->CR1 &= ~(USART_CR1_M_Msk); // M=0 corresponds to 8 data bits
    USART2->CR2 &= ~(USART_CR2_STOP_Msk); // 0b00 corresponds to 1 stop bit
    USART2->CR1 &= ~(USART_CR1_OVER8_Msk); // Set to 16 times sampling freq
    USART2->CR3 |= (USART_CR3_DMAT_Msk);
    USART2->BRR = 0;
    USART2->BRR |= (45 << USART_BRR_DIV_Mantissa_Pos); // I forget why but somehow this works with the 84MHz sysclk
    USART2->BRR |= (0b1001 << USART_BRR_DIV_Fraction_Pos); // 9/16
    USART2->CR1 |= (USART_CR1_TE_Msk); // Enable USART2
}

void configureDMA1() {
    DMA1_Stream6->CR &= ~DMA_SxCR_EN; // Disable stream to edit configuration
    while (DMA2_Stream6->CR & DMA_SxCR_EN_Msk); // wait until stream is off
    DMA1_Stream6->PAR = (uint32_t) &(USART2->DR); // destination address is USART2's transmit register
    DMA1_Stream6->CR = 0; // reset control reg to 0
    DMA1_Stream6->CR |= 4<<DMA_SxCR_CHSEL_Pos; // select channel 4
    DMA1_Stream6->CR |= 1<<DMA_SxCR_PL_Pos; // medium priority
    DMA1_Stream6->CR |= DMA_SxCR_MINC; // increment memory pointer
    DMA1_Stream6->CR |= 1<<DMA_SxCR_DIR_Pos; // memory to peripheral
}

int sendString_USART2(const char* txStr){
    /* txStr must be null-terminated */
    int txStrLen = strlen(txStr);
    if (txStrLen<1) return 1;
    // Wait if DMA is active and the last transfer has not completed.
    // For users, this means it's much better to use a large txStr
    // than to rapidly call this with small txStr's.
    while ((DMA1_Stream6->CR & DMA_SxCR_EN) &&
          ((DMA1->HISR & DMA_HISR_TCIF6)==0));
    // Launch DMA
    DMA1_Stream6->CR &= ~DMA_SxCR_EN; // disable channel
    DMA1->HIFCR |= DMA_HIFCR_CTCIF6;  // clear transfer complete flag
    DMA1_Stream6->M0AR = (uint32_t) txStr;  // set source
    DMA1_Stream6->NDTR = txStrLen;    // set length
    USART2->SR &= ~USART_SR_TC;       // clear TC flag
    DMA1_Stream6->CR |= DMA_SxCR_EN;  // enable channel
    return 0;
}










/////////
// ADC //
/////////
void configureADC() {
    // Pin 11 is x position of joystick, pin 12 is y position
    ADC->CCR |= 0b11 << ADC_CCR_ADCPRE_Pos; // adcclk = pclk2/8
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->SQR1 |= (0 << ADC_SQR1_L_Pos); // 1 conversion
    ADC1->CR2 |= ADC_CR2_EOCS; // stop after every conversion
}

uint32_t read_adc_x() {
    ADC1->SR &= ~ADC_SR_OVR;
    ADC1->SQR3 = 0;
    ADC1->SQR3 |= 11 << ADC_SQR3_SQ1_Pos; // ADC1_IN11
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}

uint32_t read_adc_y() {
    ADC1->SR &= ~ADC_SR_OVR;
    ADC1->SQR3 = 0;
    ADC1->SQR3 |= 12 << ADC_SQR3_SQ1_Pos; // ADC1_IN12
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}









//////////////////////////////
// Main Initialize Function //
//////////////////////////////
void initalize_embedded_system() {
    /* Max Speed */
    configure84MHzClock();

    /* Enable Interrupts */
    __enable_irq();

    /* Configure SPI */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    configureSPI(SPI1);
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    configureSPI(SPI3);
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    configureSoundSPI(SPI2);

    /* Configure Delay Timer */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    configureTimer(TIM3);

    /* Configure Vector Timers */
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    // Configure VEC_CLK Timer as a master clock
    configureCaptureCompare(VEC_MASTER_CLK);
    configurePWM(VEC_MASTER_CLK, 1);
    generatePWMfreq(VEC_MASTER_CLK, 10000000U, 2U);
    // Configure VEC_TIMER Timer for controlling GOb
    // as a function of the number of pulses that VEC_CLK makes
    configureCaptureCompare(VEC_TIMER);
    configureDuration(VEC_TIMER, 0, 1, 0b010);
    VEC_TIMER->DIER |= TIM_DIER_UIE; // enable interrupt req. upon updating
    NVIC_EnableIRQ(TIM5_IRQn); // enable the interrupt itself; we use it to feed new vectors

    /* Configure USART2 */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    configureUSART2();

    /* Configure DMA1 to feed USART2_TX */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    configureDMA1();

    /* Configure ADC for X and Y joysticks */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;
    configureADC();

    /* Configure GPIO */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    // USART2 GPIOA Signals
    alternateFunctionMode(USART_TX_GPIO, USART_TX_PIN, 7); // USART2_TX 
    alternateFunctionMode(USART_RX_GPIO, USART_RX_PIN, 7); // USART2_RX
    // Vector Gen Timer Signals
    alternateFunctionMode(VEC_CLK_GPIO, VEC_CLK_PIN, 2); // VEC_CLK_CH1
    alternateFunctionMode(GOb_GPIO, GOb_PIN, 2);         // VEC_TIMER_CH1
    // Vector Gen Controls
    pinMode(COUNT_LDb_GPIO, COUNT_LDb_PIN, GPIO_OUTPUT);
    pinMode(COLOR_LD_GPIO, COLOR_LD_PIN, GPIO_OUTPUT);
    pinMode(BLANKb_GPIO, BLANKb_PIN, GPIO_OUTPUT);
    // X SPI Signals
    alternateFunctionMode(X_SHIFT_REG_CLK_GPIO, X_SHIFT_REG_CLK_PIN, 5);    // SPI1_SCK
    alternateFunctionMode(X_SHIFT_REG_DATA_GPIO, X_SHIFT_REG_DATA_PIN, 5);  // SPI1_MOSI
    pinMode(X_SHIFT_REG_LD_GPIO, X_SHIFT_REG_LD_PIN, GPIO_OUTPUT);
    // Y SPI signals
    alternateFunctionMode(Y_SHIFT_REG_CLK_GPIO, Y_SHIFT_REG_CLK_PIN, 6);   // SPI3_SCK
    alternateFunctionMode(Y_SHIFT_REG_DATA_GPIO, Y_SHIFT_REG_DATA_PIN, 6); // SPI3_MOSI
    pinMode(Y_SHIFT_REG_LD_GPIO, Y_SHIFT_REG_LD_PIN, GPIO_OUTPUT);
    // Sound SPI signals
    alternateFunctionMode(SOUND_SHIFT_REG_CLK_GPIO, SOUND_SHIFT_REG_CLK_PIN, 5);   // SPI2_SCK
    alternateFunctionMode(SOUND_SHIFT_REG_DATA_GPIO, SOUND_SHIFT_REG_DATA_PIN, 5); // SPI2_MOSI
    alternateFunctionMode(SOUND_SHIFT_REG_NSS_GPIO, SOUND_SHIFT_REG_NSS_PIN, 5);   // SPI2_NSS
    // Analog Joystick signals
    pinMode(JOYSTICK_X_GPIO, JOYSTICK_X_PIN, GPIO_ANALOG); // ADC1_IN11
    pinMode(JOYSTICK_Y_GPIO, JOYSTICK_Y_PIN, GPIO_ANALOG); // ADC1_IN12
    pinMode(JOYSTICK_BTN_GPIO, JOYSTICK_BTN_PIN, GPIO_INPUT_PULLUP);
}