// gpio.h 
// This is meant to be a general-purpose upgrade/extension to the CMSIS interface for GPIOs on the STM32F401RE

#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f4xx.h"

// Logic Levels
#define GPIO_LOW    0
#define GPIO_HIGH   1

// Arbitrary GPIO functions for pinMode()
#define GPIO_INPUT        0
#define GPIO_INPUT_PULLUP 1
#define GPIO_OUTPUT       2
#define GPIO_ALT          3
#define GPIO_ANALOG       4

// Pin definitions for every GPIO pin
#define PIN_A0    0
#define PIN_A1    1
#define PIN_A2    2
#define PIN_A3    3
#define PIN_A4    4
#define PIN_A5    5
#define PIN_A6    6
#define PIN_A7    7
#define PIN_A8    8
#define PIN_A9    9
#define PIN_A10   10
#define PIN_A11   11
#define PIN_A12   12
#define PIN_A13   13
#define PIN_A14   14
#define PIN_A15   15

#define PIN_B0    0
#define PIN_B1    1
#define PIN_B2    2
#define PIN_B3    3
#define PIN_B4    4
#define PIN_B5    5
#define PIN_B6    6
#define PIN_B7    7
#define PIN_B8    8
#define PIN_B9    9
#define PIN_B10   10
#define PIN_B11   11
#define PIN_B12   12
#define PIN_B13   13
#define PIN_B14   14
#define PIN_B15   15

#define PIN_C0    0
#define PIN_C1    1
#define PIN_C2    2
#define PIN_C3    3
#define PIN_C4    4
#define PIN_C5    5
#define PIN_C6    6
#define PIN_C7    7
#define PIN_C8    8
#define PIN_C9    9
#define PIN_C10   10
#define PIN_C11   11
#define PIN_C12   12
#define PIN_C13   13
#define PIN_C14   14
#define PIN_C15   15

void pinMode(GPIO_TypeDef * GPIOx, int pin, int function);
void alternateFunctionMode(GPIO_TypeDef * GPIOx, int pin, int alt_func);
int digitalRead(GPIO_TypeDef * GPIOx, int pin);
void digitalWrite(GPIO_TypeDef * GPIOx, int pin, int val);
void togglePin(GPIO_TypeDef * GPIOx, int pin);

#endif
