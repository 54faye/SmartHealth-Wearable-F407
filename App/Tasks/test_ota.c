/*
 * OTA功能测试
 * 在串口输入对应数字执行测试
 */
#include "bsp_spi1.h"
#include "bsp_w25q128.h"
#include "bsp_at24c02.h"
#include "bsp_usart3.h"
#include "esp8266.h"
#include "ota_manager.h"
#include "bsp_usart.h"
#include "bsp_delay.h"
#include <stdio.h>
#include <string.h>

/* 测试1: W25Q128读写 */
void Test_W25Q128(void)
{
    uint32_t id;
    uint8_t w_buf[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                          0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10};
    uint8_t r_buf[16] = {0};

    printf("\r\n=== Test W25Q128 ===\r\n");

    id = W25Q128_ReadID();
    printf("JEDEC ID: 0x%06X (expect 0xEF4018)\r\n", (unsigned int)id);

    if (id != 0xEF4018) {
        printf("FAIL: ID mismatch!\r\n");
        return;
    }

    /* 擦除一个扇区 */
    W25Q128_EraseSector(0);
    printf("Sector 0 erased\r\n");

    /* 写入测试数据 */
    W25Q128_Write(0, w_buf, 16);
    printf("Written 16 bytes\r\n");

    /* 读回验证 */
    W25Q128_Read(0, r_buf, 16);

    if (memcmp(w_buf, r_buf, 16) == 0) {
        printf("PASS: W25Q128 read/write OK\r\n");
    } else {
        printf("FAIL: data mismatch!\r\n");
        printf("Read: ");
        for (int i = 0; i < 16; i++) printf("%02X ", r_buf[i]);
        printf("\r\n");
    }
}

/* 测试2: AT24C02读写 */
void Test_AT24C02(void)
{
    BootFlags_t flags;
    BootFlags_t readback;

    printf("\r\n=== Test AT24C02 ===\r\n");

    /* 写入测试标志 */
    flags.magic = OTA_MAGIC;
    flags.active_partition = 0;
    flags.ota_pending = 0;
    flags.ota_target = 1;
    flags.fw_length = 12345;
    flags.fw_crc32 = 0xDEADBEEF;
    strcpy((char *)flags.fw_version, "TEST-V1.0");

    AT24C02_WriteBootFlags(&flags);
    printf("BootFlags written\r\n");

    /* 读回验证 */
    AT24C02_ReadBootFlags(&readback);

    if (readback.magic == OTA_MAGIC &&
        readback.active_partition == 0 &&
        readback.fw_length == 12345 &&
        readback.fw_crc32 == 0xDEADBEEF) {
        printf("PASS: AT24C02 read/write OK\r\n");
        printf("  Version: %s\r\n", readback.fw_version);
    } else {
        printf("FAIL: data mismatch!\r\n");
        printf("  Magic: 0x%08X\r\n", (unsigned int)readback.magic);
        printf("  Length: %d\r\n", (int)readback.fw_length);
    }
}

/* 测试3a: ESP8266 通信诊断 */
void Test_ESP8266_AT(void)
{
    printf("\r\n=== Test ESP8266 AT ===\r\n");

    /* 固定115200初始化 */
    USART3_Init(115200);
    printf("USART3 115200 init done\r\n");

    /* 第一步: 静默监听5秒，看ESP8266是否有自发数据(启动信息) */
    printf("[1] Listening 3s for boot msg...\r\n");
    USART3_FlushRx();
    Delay_ms(3000);
    uint16_t count = USART3_GetRxCount();
    printf("    Received %d bytes\r\n", count);
    if (count > 0) {
        printf("    Data: [");
        while (USART3_GetRxCount() > 0) {
            uint8_t ch = USART3_ReadByte();
            if (ch >= 0x20 && ch < 0x7F) {
                printf("%c", ch);
            } else {
                printf("<%02X>", ch);
            }
        }
        printf("]\r\n");
    }

    /* 第二步: 发送AT，精确统计收到的字节 */
    printf("[2] Sending AT, waiting 2s...\r\n");
    USART3_FlushRx();
    USART3_SendString("AT\r\n");
    printf("    Sent 4 bytes: A T \\r \\n\r\n");

    count = 0;
    uint32_t timeout = 0;
    uint8_t resp[64];
    uint8_t ridx = 0;

    while (timeout < 2000) {
        if (USART3_GetRxCount() > 0) {
            uint8_t ch = USART3_ReadByte();
            if (ridx < sizeof(resp) - 1) {
                resp[ridx++] = ch;
            }
            count++;
        }
        Delay_ms(1);
        timeout++;
    }
    resp[ridx] = '\0';

    printf("    Received %d bytes in %d ms\r\n", count, (int)timeout);
    if (ridx > 0) {
        printf("    Hex: ");
        for (uint8_t i = 0; i < ridx; i++) {
            printf("%02X ", resp[i]);
        }
        printf("\r\n");
        printf("    Chr: [");
        for (uint8_t i = 0; i < ridx; i++) {
            if (resp[i] >= 0x20 && resp[i] < 0x7F) {
                printf("%c", resp[i]);
            } else {
                printf("<%02X>", resp[i]);
            }
        }
        printf("]\r\n");
    }

    /* 判断: 在响应中搜索 "OK" */
    uint8_t found_ok = 0;
    for (uint8_t i = 0; i + 1 < ridx; i++) {
        if (resp[i] == 'O' && resp[i+1] == 'K') {
            found_ok = 1;
            break;
        }
    }

    if (found_ok) {
        printf("PASS: ESP8266 AT OK at 115200 baud\r\n");
    } else if (count == 0) {
        printf("FAIL: ESP8266 not responding\r\n");
        printf("  -> Check wiring: ESP8266 TX->PB10, RX->PB11\r\n");
        printf("  -> Check power: ESP8266 needs 3.3V, 200mA+\r\n");
    } else {
        printf("FAIL: Got %d bytes but no OK\r\n", count);
    }
}

/* 测试3b: ESP8266 WiFi连接 (需要修改SSID和密码) */
void Test_ESP8266(void)
{
    printf("\r\n=== Test ESP8266 WiFi ===\r\n");

    ESP8266_Init();
    printf("ESP8266 init done\r\n");

    /* 修改为你实际的WiFi名称和密码 */
    printf("Connecting to WiFi...\r\n");
    uint8_t ret = ESP8266_ConnectWiFi("wu", "wsf123456");

    if (ret == 0) {
        printf("PASS: WiFi connected!\r\n");
    } else {
        printf("FAIL: WiFi connect error %d\r\n", ret);
    }
}

/* 测试4: 设置OTA标志 (模拟OTA完成，重启后Bootloader会处理) */
void Test_SetOTAFlag(void)
{
    printf("\r\n=== Set OTA Flag (next boot will flash) ===\r\n");
    printf("WARNING: This requires valid firmware in W25Q128!\r\n");

    OTA_SetBootFlag(1024, 0x12345678, "TEST-V1.0");
    printf("OTA flag set. Reboot to trigger OTA.\r\n");
    printf("Current active: Partition %s\r\n",
           OTA_GetActivePartition() == 0 ? "A" : "B");
}

/* 测试5: 完整OTA流程 (下载→W25Q128→校验→设置标志→重启) */
void Test_OTA_Full(void)
{
    printf("\r\n=== Full OTA Test ===\r\n");
    printf("Server: %s:%d%s\r\n", OTA_SERVER_IP, OTA_SERVER_PORT, OTA_URL_PATH);

    /* 1. 连接WiFi */
    printf("[1] Connecting WiFi...\r\n");
    ESP8266_Init();
    if (ESP8266_ConnectWiFi("wu", "wsf123456") != 0) {
        printf("FAIL: WiFi connect error\r\n");
        return;
    }
    printf("    WiFi OK\r\n");

    /* 2. 下载固件到W25Q128 */
    printf("[2] Downloading firmware...\r\n");
    uint8_t ret = OTA_DownloadFirmware(OTA_SERVER_IP, OTA_SERVER_PORT, OTA_URL_PATH);
    if (ret != 0) {
        printf("FAIL: Download error %d\r\n", ret);
        return;
    }
    uint32_t fw_len = OTA_GetLastDownloadLen();
    printf("    Download OK, %d bytes\r\n", (int)fw_len);

    if (fw_len == 0 || fw_len > OTA_W25Q_MAX_SIZE) {
        printf("FAIL: Invalid firmware length\r\n");
        return;
    }

    /* 3. 验证固件头 */
    printf("[3] Verifying header...\r\n");
    uint8_t header[4];
    W25Q128_Read(0, header, 4);
    uint32_t sp = header[0] | (header[1] << 8) | (header[2] << 16) | (header[3] << 24);
    if ((sp & 0x2FF00000) != 0x20000000) {
        printf("FAIL: Not valid firmware (SP=0x%08X)\r\n", (unsigned int)sp);
        return;
    }
    printf("    Valid (SP=0x%08X)\r\n", (unsigned int)sp);

    /* 4. 计算CRC32 */
    printf("[4] Computing CRC32...\r\n");
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    CRC_ResetDR();

    uint32_t addr = 0;
    uint32_t remain = fw_len;
    uint8_t crc_buf[256];

    while (remain > 0) {
        uint32_t chunk = (remain > 256) ? 256 : remain;
        W25Q128_Read(addr, crc_buf, chunk);
        uint32_t word_count = chunk / 4;
        for (uint32_t i = 0; i < word_count; i++) {
            uint32_t word;
            memcpy(&word, &crc_buf[i * 4], 4);
            CRC_CalcCRC(word);
        }
        uint32_t rem = chunk % 4;
        if (rem > 0) {
            uint32_t last_word = 0xFFFFFFFF;
            memcpy(&last_word, &crc_buf[word_count * 4], rem);
            CRC_CalcCRC(last_word);
        }
        addr += chunk;
        remain -= chunk;
    }
    uint32_t crc = CRC_GetCRC();
    printf("    CRC32: 0x%08X\r\n", (unsigned int)crc);

    /* 5. 设置OTA标志 */
    printf("[5] Setting OTA flag...\r\n");
    OTA_SetBootFlag(fw_len, crc, "OTA-V2.0");
    printf("    Active: Partition %s\r\n",
           OTA_GetActivePartition() == 0 ? "A" : "B");

    printf("\r\n=== OTA Ready! Reboot to flash. ===\r\n");
}

/* 测试入口 - 在main.c的StartTask中调用 */
void OTA_TestMenu(void)
{
    printf("\r\n=======================\r\n");
    printf("OTA Test Menu:\r\n");
    printf("  1: Test W25Q128\r\n");
    printf("  2: Test AT24C02\r\n");
    printf("  3: Test ESP8266 AT\r\n");
    printf("  4: Test ESP8266 WiFi\r\n");
    printf("  5: Set OTA Flag\r\n");
    printf("  6: Full OTA Test\r\n");
    printf("=======================\r\n");
}
