#ifndef __BSP_SPI1_H
#define __BSP_SPI1_H

#include "stm32f4xx.h"

/* 引脚定义 */
#define SPI1_SCK_GPIO      GPIOA
#define SPI1_SCK_PIN       GPIO_Pin_5
#define SPI1_SCK_SOURCE    GPIO_PinSource5

#define SPI1_MISO_GPIO     GPIOA
#define SPI1_MISO_PIN      GPIO_Pin_6
#define SPI1_MISO_SOURCE   GPIO_PinSource6

#define SPI1_MOSI_GPIO     GPIOA
#define SPI1_MOSI_PIN      GPIO_Pin_7
#define SPI1_MOSI_SOURCE   GPIO_PinSource7

#define SPI1_CS_GPIO       GPIOC
#define SPI1_CS_PIN        GPIO_Pin_13

/* CS控制宏 */
#define SPI1_CS_LOW()      (SPI1_CS_GPIO->BSRR = SPI1_CS_PIN << 16)
#define SPI1_CS_HIGH()     (SPI1_CS_GPIO->BSRR = SPI1_CS_PIN)

void SPI1_Init(void);
uint8_t SPI1_ReadWriteByte(uint8_t tx_data);

#endif
