#ifndef __OTA_MANAGER_H
#define __OTA_MANAGER_H

#include "stm32f4xx.h"

/* OTA服务器配置 */
#define OTA_SERVER_IP     "192.168.1.3"
#define OTA_SERVER_PORT   9090
#define OTA_URL_PATH      "/SmartWatch_F407.bin"

/* W25Q128中OTA固件暂存区 */
#define OTA_W25Q_ADDR     0x000000
#define OTA_W25Q_MAX_SIZE (384*1024)  /* 384KB */

uint8_t OTA_DownloadFirmware(char *server_ip, uint16_t port, char *url_path);
uint32_t OTA_GetLastDownloadLen(void);
uint8_t OTA_VerifyFirmware(uint32_t expected_len, uint32_t expected_crc);
void OTA_SetBootFlag(uint32_t fw_length, uint32_t fw_crc, char *version);
uint8_t OTA_GetActivePartition(void);
void OTA_SwitchPartition(void);
void OTA_TriggerReboot(void);

#endif
