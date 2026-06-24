#ifndef __STEP_COUNTER_H
#define __STEP_COUNTER_H

#include <stdint.h>

typedef struct {
    uint32_t TotalSteps;
    uint32_t TodaySteps;
} StepData_t;

void StepCounter_Init(void);
void StepCounter_Update(int16_t ax, int16_t ay, int16_t az);
uint32_t StepCounter_GetTotalSteps(void);
uint32_t StepCounter_GetTodaySteps(void);
void StepCounter_ResetToday(void);

#endif
