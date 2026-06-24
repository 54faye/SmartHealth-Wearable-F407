#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "stm32f4xx.h"

void Scheduler_Init(void);
void Scheduler_Run(void);

void DelayMS(uint32_t ms);
void DelayUS(uint32_t us);
uint32_t Scheduler_GetTimeInterval(void);

#endif
