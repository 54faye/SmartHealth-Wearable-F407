#ifndef __PEDOMETER_H
#define __PEDOMETER_H

#include <stdint.h>

void Pedometer_Init(void);
void Pedometer_Update(int16_t ax, int16_t ay, int16_t az, uint32_t current_tick);
uint32_t Pedometer_GetStepCount(void);
void Pedometer_Reset(void);

#endif
