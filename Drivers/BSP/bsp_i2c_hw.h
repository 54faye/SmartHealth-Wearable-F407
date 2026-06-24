#ifndef __BSP_I2C_HW_H
#define __BSP_I2C_HW_H

#include "stm32f4xx.h"

void HW_I2C1_Init(void);
uint8_t HW_I2C1_WriteBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
uint8_t HW_I2C1_ReadBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

#endif
