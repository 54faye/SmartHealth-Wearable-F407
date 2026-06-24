#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f4xx.h"

/* 移植配置 */
#define LED_RED_GPIO      GPIOC
#define LED_RED_PIN       GPIO_Pin_5

#define LED_BLUE_GPIO     GPIOB
#define LED_BLUE_PIN      GPIO_Pin_2

/* 简化操作宏 (低电平点亮) */
#define LED_RED_ON         (LED_RED_GPIO->BSRR = LED_RED_PIN << 16)
#define LED_RED_OFF        (LED_RED_GPIO->BSRR = LED_RED_PIN)
#define LED_RED_TOGGLE     (LED_RED_GPIO->ODR  ^= LED_RED_PIN)

#define LED_BLUE_ON        (LED_BLUE_GPIO->BSRR = LED_BLUE_PIN << 16)
#define LED_BLUE_OFF       (LED_BLUE_GPIO->BSRR = LED_BLUE_PIN)
#define LED_BLUE_TOGGLE    (LED_BLUE_GPIO->ODR  ^= LED_BLUE_PIN)

/* 兼容原项目的函数接口 */
void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);

#endif
