#ifndef __BSP_AT24C02_H
#define __BSP_AT24C02_H

#include "stm32f4xx.h"

/* AT24C02 I2C地址 (7bit: 0x50) */
#define AT24C02_ADDR_WRITE  0xA0
#define AT24C02_ADDR_READ   0xA1

/* OTA魔数 */
#define OTA_MAGIC           0x4F544131  /* "OTA1" */

/* 启动标志结构体 (32字节，存在AT24C02地址0x00) */
typedef struct {
    uint32_t magic;              /* OTA_MAGIC */
    uint8_t  active_partition;   /* 0=分区A, 1=分区B */
    uint8_t  ota_pending;        /* 1=有待更新固件 */
    uint8_t  ota_target;         /* 0=目标A, 1=目标B */
    uint8_t  reserved;
    uint32_t fw_length;          /* 固件长度(字节) */
    uint32_t fw_crc32;           /* 固件CRC32校验 */
    uint8_t  fw_version[16];     /* 版本号字符串 */
} BootFlags_t;

/* 分区地址定义 */
#define PARTITION_A_ADDR    0x08010000
#define PARTITION_B_ADDR    0x08040000
#define PARTITION_A_SIZE    (192*1024)   /* 192KB: Sector4(64K) + Sector5(128K) */
#define PARTITION_B_SIZE    (256*1024)   /* 256KB: Sector6(128K) + Sector7(128K) */

void AT24C02_Init(void);
void AT24C02_WriteByte(uint8_t addr, uint8_t data);
uint8_t AT24C02_ReadByte(uint8_t addr);
void AT24C02_Write(uint8_t addr, uint8_t *data, uint8_t len);
void AT24C02_Read(uint8_t addr, uint8_t *data, uint8_t len);
void AT24C02_WriteBootFlags(BootFlags_t *flags);
void AT24C02_ReadBootFlags(BootFlags_t *flags);
void AT24C02_ClearOTAPending(void);

#endif
