#include "bsp_delay.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

static uint16_t g_fac_us = 0;

void SysTick_Init(uint16_t sysclk)
{
    uint32_t reload;

    SysTick->CTRL = 0;

    /* CLKSOURCE=1, 使用处理器时钟 (168MHz) */
    SysTick->CTRL |= (1 << 2);

    g_fac_us = sysclk;

    reload = sysclk;
    reload *= 1000000 / configTICK_RATE_HZ;

    SysTick->LOAD = reload;
    SysTick->CTRL |= (1 << 1);  /* TICKINT=1 */
    SysTick->CTRL |= (1 << 0);  /* ENABLE=1 */
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
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks) break;
        }
    }
}

void Delay_ms(uint16_t ms)
{
    uint32_t i;

    for (i = 0; i < ms; i++)
    {
        Delay_us(1000);
    }
}

void Delay_s(uint16_t s)
{
    uint32_t i;

    for (i = 0; i < s; i++)
    {
        Delay_ms(1000);
    }
}

extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}
