#ifndef __BSP_W25Q128_H
#define __BSP_W25Q128_H

#include "stm32f4xx.h"

#define W25Q_PAGE_SIZE      256
#define W25Q_SECTOR_SIZE    4096
#define W25Q_BLOCK64K_SIZE  (64*1024)

void W25Q128_Init(void);
uint32_t W25Q128_ReadID(void);
void W25Q128_EraseSector(uint32_t sector_addr);
void W25Q128_EraseBlock64K(uint32_t block_addr);
void W25Q128_Write(uint32_t addr, uint8_t *buf, uint32_t len);
void W25Q128_Read(uint32_t addr, uint8_t *buf, uint32_t len);

#endif
