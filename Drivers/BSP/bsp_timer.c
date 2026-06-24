#include "stm32f4xx.h"
#include "bsp_timer.h"
#include "DataCenter.h"

uint32_t FreeRTOSRunTimeTicks = 0;

void Timer_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 840 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

void ConfigureTimeForRunTimeStats(void)
{
    FreeRTOSRunTimeTicks = 0;
}

void TIM2_IRQHandler(void)
{
    extern void Key_Tick(void);
    extern void Score_Tick(void);

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Key_Tick();
        DataCenter_StopWatch_Tick();
        Score_Tick();
        FreeRTOSRunTimeTicks++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
