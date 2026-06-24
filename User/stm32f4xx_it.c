/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines
  ******************************************************************************
  */

#include "stm32f4xx_it.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1) {}
}

void MemManage_Handler(void)
{
    while (1) {}
}

void BusFault_Handler(void)
{
    while (1) {}
}

void UsageFault_Handler(void)
{
    while (1) {}
}

void DebugMon_Handler(void)
{
}

/* SVC_Handler, PendSV_Handler, SysTick_Handler 由FreeRTOS和bsp_delay.c提供 */
