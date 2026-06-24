#include "ota_manager.h"
#include "esp8266.h"
#include "bsp_w25q128.h"
#include "bsp_at24c02.h"
#include "bsp_delay.h"
#include <string.h>
#include <stdio.h>

/*
 * 通过HTTP下载固件到W25Q128
 * 返回: 0=成功, 其他=失败
 */
static uint32_t last_download_len = 0;

uint8_t OTA_DownloadFirmware(char *server_ip, uint16_t port, char *url_path)
{
    char http_req[256];
    uint8_t recv_buf[1500];
    uint32_t recv_len;
    uint32_t w25q_addr = OTA_W25Q_ADDR;

    last_download_len = 0;

    /* 1. 连接TCP服务器 */
    if (ESP8266_ConnectTCP(server_ip, port) != 0) {
        return 1;
    }
    Delay_ms(200);

    /* 2. 发送HTTP GET请求 */
    sprintf(http_req,
        "GET %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "Connection: close\r\n"
        "\r\n",
        url_path, server_ip, port);

    if (ESP8266_SendData((uint8_t *)http_req, strlen(http_req)) != 0) {
        return 2;
    }

    /* 3. 接收HTTP响应并写入W25Q128 */
    /* 先擦除OTA暂存区 (按64KB块擦除) */
    for (uint32_t blk = 0; blk < OTA_W25Q_MAX_SIZE; blk += 64*1024) {
        W25Q128_EraseBlock64K(blk);
    }

    /* 流式接收并写入W25Q128 */
    while (1) {
        recv_len = ESP8266_ReceiveHTTP(recv_buf, sizeof(recv_buf), 30000);
        if (recv_len == 0) break;

        W25Q128_Write(w25q_addr, recv_buf, recv_len);
        w25q_addr += recv_len;

        if (w25q_addr >= OTA_W25Q_ADDR + OTA_W25Q_MAX_SIZE) {
            return 3;
        }
    }

    ESP8266_CloseTCP();

    last_download_len = w25q_addr - OTA_W25Q_ADDR;
    return 0;
}

uint32_t OTA_GetLastDownloadLen(void)
{
    return last_download_len;
}

/*
 * 从W25Q128读回固件并校验CRC32
 * 返回: 0=校验通过, 1=长度不匹配, 2=CRC不匹配
 */
uint8_t OTA_VerifyFirmware(uint32_t expected_len, uint32_t expected_crc)
{
    uint8_t buf[256];
    uint32_t addr = OTA_W25Q_ADDR;
    uint32_t remaining = expected_len;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    CRC_ResetDR();

    while (remaining > 0) {
        uint32_t chunk = (remaining > 256) ? 256 : remaining;
        W25Q128_Read(addr, buf, chunk);

        /* 按32位计算CRC */
        uint32_t word_count = chunk / 4;
        for (uint32_t i = 0; i < word_count; i++) {
            uint32_t word;
            memcpy(&word, &buf[i * 4], 4);
            CRC_CalcCRC(word);
        }

        uint32_t rem = chunk % 4;
        if (rem > 0) {
            uint32_t last_word = 0xFFFFFFFF;
            memcpy(&last_word, &buf[word_count * 4], rem);
            CRC_CalcCRC(last_word);
        }

        addr += chunk;
        remaining -= chunk;
    }

    uint32_t crc = CRC_GetCRC();

    if (crc != expected_crc) {
        return 2;
    }

    return 0;
}

/*
 * 设置OTA启动标志
 * 将目标分区信息写入AT24C02
 */
void OTA_SetBootFlag(uint32_t fw_length, uint32_t fw_crc, char *version)
{
    BootFlags_t flags;

    AT24C02_ReadBootFlags(&flags);

    /* 如果magic无效，初始化默认值 */
    if (flags.magic != OTA_MAGIC) {
        flags.magic = OTA_MAGIC;
        flags.active_partition = 0;  /* 默认分区A */
    }

    /* 设置OTA目标为非活动分区 */
    flags.ota_target = (flags.active_partition == 0) ? 1 : 0;
    flags.ota_pending = 1;
    flags.fw_length = fw_length;
    flags.fw_crc32 = fw_crc;
    memset(flags.fw_version, 0, sizeof(flags.fw_version));
    if (version != NULL) {
        strncpy((char *)flags.fw_version, version, sizeof(flags.fw_version) - 1);
    }

    AT24C02_WriteBootFlags(&flags);
}

/*
 * 获取当前活动分区
 * 返回: 0=分区A(0x08010000), 1=分区B(0x08040000)
 */
uint8_t OTA_GetActivePartition(void)
{
    BootFlags_t flags;
    AT24C02_ReadBootFlags(&flags);

    if (flags.magic != OTA_MAGIC) {
        return 0;  /* 默认分区A */
    }

    return flags.active_partition;
}

/*
 * 切换活动分区 (在Bootloader中成功更新后调用)
 */
void OTA_SwitchPartition(void)
{
    BootFlags_t flags;
    AT24C02_ReadBootFlags(&flags);

    if (flags.magic != OTA_MAGIC) {
        flags.magic = OTA_MAGIC;
        flags.active_partition = 0;
    }

    /* 切换到OTA目标分区 */
    flags.active_partition = flags.ota_target;
    flags.ota_pending = 0;

    AT24C02_WriteBootFlags(&flags);
}

/*
 * 触发系统重启
 */
void OTA_TriggerReboot(void)
{
    NVIC_SystemReset();
}
