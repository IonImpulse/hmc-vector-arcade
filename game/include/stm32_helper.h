// stm32_helper.h
// Provides functions for driving the various peripherals of the STM32F401RE
// These functions are NOT generalizeable: they are specific to the vector arcade project.
#ifndef __STM32_HELPER_H__
#define __STM32_HELPER_H__

#include "stm32f4xx.h"
#include "gpio.h"

// Pin Assignments
#define VEC_CLK_PIN               PIN_B6  // white wire
#define VEC_CLK_GPIO              GPIOB
#define GOb_PIN                   PIN_A0  // green wire
#define GOb_GPIO                  GPIOA
#define COUNT_LDb_PIN             PIN_A4  // blue wire
#define COUNT_LDb_GPIO            GPIOA
#define COLOR_LD_PIN              PIN_A8  // yellow wire
#define COLOR_LD_GPIO             GPIOA
#define BLANKb_PIN                PIN_B4  // brown wire
#define BLANKb_GPIO               GPIOB
#define X_SHIFT_REG_CLK_PIN       PIN_A5  // orange wire
#define X_SHIFT_REG_CLK_GPIO      GPIOA
#define X_SHIFT_REG_LD_PIN        PIN_A6  // red wire
#define X_SHIFT_REG_LD_GPIO       GPIOA
#define X_SHIFT_REG_DATA_PIN      PIN_A7  // brown wire
#define X_SHIFT_REG_DATA_GPIO     GPIOA
#define Y_SHIFT_REG_CLK_PIN       PIN_C10 // orange wire
#define Y_SHIFT_REG_CLK_GPIO      GPIOC
#define Y_SHIFT_REG_LD_PIN        PIN_C11 // red wire
#define Y_SHIFT_REG_LD_GPIO       GPIOC
#define Y_SHIFT_REG_DATA_PIN      PIN_C12 // brown wire
#define Y_SHIFT_REG_DATA_GPIO     GPIOC
#define SOUND_SHIFT_REG_CLK_PIN   PIN_B10 // white wire
#define SOUND_SHIFT_REG_CLK_GPIO  GPIOB
#define SOUND_SHIFT_REG_NSS_PIN   PIN_B12 // green wire in socket
#define SOUND_SHIFT_REG_NSS_GPIO  GPIOB
#define SOUND_SHIFT_REG_DATA_PIN  PIN_B15 // yellow wire in socket
#define SOUND_SHIFT_REG_DATA_GPIO GPIOB
#define USART_TX_PIN              PIN_A2  // internally connected to ST-LINK
#define USART_TX_GPIO             GPIOA
#define USART_RX_PIN              PIN_A3  // internally connected to ST-LINK
#define USART_RX_GPIO             GPIOA
#define JOYSTICK_X_PIN            PIN_C1
#define JOYSTICK_X_GPIO           GPIOC
#define JOYSTICK_Y_PIN            PIN_C2
#define JOYSTICK_Y_GPIO           GPIOC
#define JOYSTICK_BTN_PIN          PIN_C0
#define JOYSTICK_BTN_GPIO         GPIOC

// Peripheral Assignments
#define USART           USART2
#define DELAY_TIM       TIM3
#define VEC_MASTER_CLK  TIM4
#define VEC_TIMER       TIM5
#define X_SPI           SPI1
#define Y_SPI           SPI3
#define SOUND_SPI       SPI2
#define JOYSTICK_ADC    ADC1

void initalize_embedded_system();
void SPIsend(SPI_TypeDef * SPIx, uint16_t data);
int sendString_USART2(const char* txStr);
void generateDuration(TIM_TypeDef * TIMx, uint32_t duration, uint32_t compare_val);
void start_micros(TIM_TypeDef * TIMx, uint32_t us);
bool tim_done(TIM_TypeDef * TIMx);
uint32_t read_adc_x();
uint32_t read_adc_y();
#endif