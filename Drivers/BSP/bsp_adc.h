#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f4xx.h"

void AD_Init(void);
uint16_t AD_GetValue(void);

#endif
