#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include <stdint.h>

void Timer_Init(void);
void ConfigureTimeForRunTimeStats(void);
extern uint32_t FreeRTOSRunTimeTicks;

#endif
