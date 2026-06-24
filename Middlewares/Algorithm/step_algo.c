#include "step_algo.h"
#include <math.h>
#include <stdlib.h>

/* 计步算法参数 */
#define STEP_THRESHOLD_MIN  2000  // 最小加速度变化阈值 (根据实际传感器灵敏度调整)
#define STEP_THRESHOLD_MAX  8000  // 最大加速度变化阈值
#define STEP_INTERVAL_MIN   200   // 最小步数间隔 (ms)
#define STEP_INTERVAL_MAX   2000  // 最大步数间隔 (ms)

static StepData_t StepData = {0, 0};
static uint32_t LastStepTime = 0;
static float OldAcceleration = 0;

/* 外部依赖：获取系统滴答计数 (ms) */
extern uint32_t xTaskGetTickCount(void); 

void StepCounter_Init(void)
{
    StepData.TotalSteps = 0;
    StepData.TodaySteps = 0;
}

/**
  * @brief  更新计步器 (需周期性调用，例如50ms一次)
  * @param  ax, ay, az: 三轴加速度原始数据
  */
void StepCounter_Update(int16_t ax, int16_t ay, int16_t az)
{
    /* 计算合加速度 */
    float CurrentAcceleration = sqrt(ax * ax + ay * ay + az * az);
    
    /* 简单的波峰检测 */
    // 实际应用中可能需要更复杂的滤波和动态阈值
    
    if (OldAcceleration == 0) {
        OldAcceleration = CurrentAcceleration;
        return;
    }
    
    float Delta = CurrentAcceleration - OldAcceleration;
    
    /* 获取当前时间 (FreeRTOS Tick) */
    uint32_t CurrentTime = xTaskGetTickCount(); // 假设1Tick=1ms
    
    if (Delta > STEP_THRESHOLD_MIN && Delta < STEP_THRESHOLD_MAX)
    {
        if (CurrentTime - LastStepTime > STEP_INTERVAL_MIN && CurrentTime - LastStepTime < STEP_INTERVAL_MAX)
        {
            StepData.TotalSteps++;
            StepData.TodaySteps++;
            LastStepTime = CurrentTime;
        }
    }
    
    OldAcceleration = CurrentAcceleration;
}

uint32_t StepCounter_GetTotalSteps(void)
{
    return StepData.TotalSteps;
}

uint32_t StepCounter_GetTodaySteps(void)
{
    return StepData.TodaySteps;
}

void StepCounter_ResetToday(void)
{
    StepData.TodaySteps = 0;
}
