#include "boot_config.h"
#include "bsp_delay.h"
#include "bsp_usart.h"
#include "bsp_spi1.h"
#include "bsp_w25q128.h"
#include "bsp_i2c_hw.h"
#include "bsp_at24c02.h"
#include <string.h>

static void uart_putc(char c)
{
    while ((USART1->SR & 0x40) == 0);
    USART1->DR = c;
}

static void uart_puts(const char *s)
{
    while (*s) uart_putc(*s++);
}

static void uart_hex8(uint8_t v)
{
    const char h[] = "0123456789ABCDEF";
    uart_putc(h[(v >> 4) & 0x0F]);
    uart_putc(h[v & 0x0F]);
}

static void uart_hex32(uint32_t v)
{
    const char h[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 28; i >= 0; i -= 4)
        uart_putc(h[(v >> i) & 0x0F]);
}

static void uart_uint(uint32_t v)
{
    char buf[11];
    int i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v > 0) { buf[i++] = '0' + (v % 10); v /= 10; }
    while (i > 0) uart_putc(buf[--i]);
}

static void JumpToApp(uint32_t app_addr)
{
    uint32_t jump_address;
    typedef void (*pFunction)(void);
    pFunction Jump_To_Application;

    if (((*(__IO uint32_t *)app_addr) & 0x2FF00000) == 0x20000000)
    {
        uart_puts("[BOOT] Jump to ");
        uart_hex32(app_addr);
        uart_puts("\r\n");

        __disable_irq();
        SysTick->CTRL = 0;
        SysTick->LOAD = 0;
        SysTick->VAL  = 0;

        __set_MSP(*(__IO uint32_t *)app_addr);
        SCB->VTOR = app_addr;

        jump_address = *(__IO uint32_t *)(app_addr + 4);
        Jump_To_Application = (pFunction)jump_address;

        __enable_irq();
        Jump_To_Application();
    }
    else
    {
        uart_puts("[BOOT] Invalid app at ");
        uart_hex32(app_addr);
        uart_puts("\r\n");
    }
}

static uint8_t FlashFirmwareFromW25Q128(uint32_t target_addr, uint32_t fw_length)
{
    uint8_t buf[4];
    uint32_t w25q_addr = 0x000000;

    uart_puts("[BOOT] Erasing flash...\r\n");

    FLASH_Unlock();

    if (target_addr == PARTITION_A_ADDR) {
        FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
        FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);
    } else if (target_addr == PARTITION_B_ADDR) {
        FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3);
        FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);
    }

    uart_puts("[BOOT] Writing ");
    uart_uint(fw_length);
    uart_puts(" bytes...\r\n");

    for (uint32_t i = 0; i < fw_length; i += 4) {
        W25Q128_Read(w25q_addr + i, buf, 4);
        uint32_t word = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

        if (FLASH_ProgramWord(target_addr + i, word) != FLASH_COMPLETE) {
            FLASH_Lock();
            uart_puts("[BOOT] Write error!\r\n");
            return 1;
        }
        FLASH_WaitForLastOperation();
    }

    FLASH_Lock();
    uart_puts("[BOOT] Write done\r\n");
    return 0;
}

static uint32_t CalcW25Q128CRC32(uint32_t length)
{
    uint8_t buf[256];
    uint32_t addr = 0;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    CRC_ResetDR();

    while (length > 0) {
        uint32_t chunk = (length > 256) ? 256 : length;
        W25Q128_Read(addr, buf, chunk);

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
        length -= chunk;
    }

    return CRC_GetCRC();
}

int main(void)
{
    BootFlags_t bootFlags;
    uint32_t target_addr;

    SysTick_Init(168);
    USART1_Init(115200);

    uart_puts("\r\n=============================\r\n");
    uart_puts("[BOOT] Bootloader v1.0\r\n");
    uart_puts("[BOOT] STM32F407VE OTA\r\n");
    uart_puts("=============================\r\n");

    uart_puts("[BOOT] Init I2C1...\r\n");
    HW_I2C1_Init();
    uart_puts("[BOOT] I2C1 OK\r\n");

    uart_puts("[BOOT] AT24C02 test...\r\n");
    {
        uint8_t test_byte = AT24C02_ReadByte(0);
        uart_puts("[BOOT] AT24C02[0]=0x");
        uart_hex8(test_byte);
        uart_puts("\r\n");
    }

    uart_puts("[BOOT] Init SPI1...\r\n");
    SPI1_Init();
    uart_puts("[BOOT] SPI1 OK\r\n");

    uart_puts("[BOOT] W25Q128 ID: ");
    uart_hex32(W25Q128_ReadID());
    uart_puts("\r\n");

    uart_puts("[BOOT] Read boot flags...\r\n");
    AT24C02_ReadBootFlags(&bootFlags);
    uart_puts("[BOOT] Flags OK\r\n");

    if (bootFlags.magic == OTA_MAGIC && bootFlags.ota_pending == 1)
    {
        uart_puts("[BOOT] OTA pending -> Partition ");
        uart_puts(bootFlags.ota_target == 0 ? "A" : "B");
        uart_puts("\r\n");

        uart_puts("[BOOT] FW len: ");
        uart_uint(bootFlags.fw_length);
        uart_puts(" ver: ");
        uart_puts((const char *)bootFlags.fw_version);
        uart_puts("\r\n");

        target_addr = (bootFlags.ota_target == 0) ? PARTITION_A_ADDR : PARTITION_B_ADDR;

        uart_puts("[BOOT] CRC check...\r\n");
        uint32_t calc_crc = CalcW25Q128CRC32(bootFlags.fw_length);
        uart_puts("[BOOT] Calc="); uart_hex32(calc_crc);
        uart_puts(" Exp="); uart_hex32(bootFlags.fw_crc32);
        uart_puts("\r\n");

        if (calc_crc == bootFlags.fw_crc32)
        {
            uart_puts("[BOOT] CRC OK, flashing...\r\n");
            if (FlashFirmwareFromW25Q128(target_addr, bootFlags.fw_length) == 0)
            {
                bootFlags.active_partition = bootFlags.ota_target;
                bootFlags.ota_pending = 0;
                AT24C02_WriteBootFlags(&bootFlags);
                uart_puts("[BOOT] OTA done! -> ");
                uart_puts(bootFlags.active_partition == 0 ? "A" : "B");
                uart_puts("\r\n");
            }
        }
        else
        {
            uart_puts("[BOOT] CRC fail!\r\n");
            bootFlags.ota_pending = 0;
            AT24C02_WriteBootFlags(&bootFlags);
        }
    }

    if (bootFlags.magic != OTA_MAGIC) {
        uart_puts("[BOOT] Default -> A\r\n");
        target_addr = PARTITION_A_ADDR;
    } else {
        target_addr = (bootFlags.active_partition == 0) ? PARTITION_A_ADDR : PARTITION_B_ADDR;
    }

    uart_puts("[BOOT] Active: ");
    uart_puts((target_addr == PARTITION_A_ADDR) ? "A" : "B");
    uart_puts(" @ "); uart_hex32(target_addr);
    uart_puts("\r\n");

    JumpToApp(target_addr);

    uart_puts("[BOOT] Jump fail!\r\n");
    while (1);
}

void SysTick_Handler(void) {}
