#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f4xx.h"

void Key_Init(void);
uint8_t Key_GetNum(void);
void Key_Tick(void);
uint8_t Key_GetState(void);

#endif
