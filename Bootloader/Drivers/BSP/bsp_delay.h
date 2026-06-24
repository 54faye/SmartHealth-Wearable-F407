#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "stm32f4xx.h"

void SysTick_Init(uint16_t sysclk);
void Delay_us(uint32_t us);
void Delay_ms(uint16_t ms);
void Delay_s(uint16_t s);

#endif
