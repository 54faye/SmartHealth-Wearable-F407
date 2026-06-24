/* 裸机版Delay驱动 (无FreeRTOS依赖) */
#include "bsp_delay.h"

static uint16_t g_fac_us = 0;

void SysTick_Init(uint16_t sysclk)
{
    SysTick->CTRL = 0;
    SysTick->CTRL |= (1 << 2);  /* CLKSOURCE=1, 处理器时钟 */
    g_fac_us = sysclk;
    SysTick->LOAD = 0xFFFFFF;    /* 最大重载值 */
    SysTick->VAL  = 0;
    SysTick->CTRL |= (1 << 0);  /* ENABLE=1, 不使能中断 */
}

void Delay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD;
    ticks = us * g_fac_us;
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += reload - tnow + told;
            told = tnow;
            if (tcnt >= ticks) break;
        }
    }
}

void Delay_ms(uint16_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
        Delay_us(1000);
}

void Delay_s(uint16_t s)
{
    for (uint32_t i = 0; i < s; i++)
        Delay_ms(1000);
}
