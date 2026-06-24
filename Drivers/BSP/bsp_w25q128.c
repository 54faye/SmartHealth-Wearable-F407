#include "bsp_w25q128.h"
#include "bsp_spi1.h"

/* W25Q128命令字 */
#define CMD_WRITE_ENABLE        0x06
#define CMD_WRITE_DISABLE       0x04
#define CMD_READ_STATUS_REG1    0x05
#define CMD_READ_DATA           0x03
#define CMD_PAGE_PROGRAM        0x02
#define CMD_SECTOR_ERASE        0x20
#define CMD_BLOCK64K_ERASE      0xD8
#define CMD_CHIP_ERASE          0xC7
#define CMD_READ_JEDEC_ID       0x9F
#define CMD_ENABLE_RESET        0x66
#define CMD_RESET               0x99

static void W25Q128_WriteEnable(void)
{
    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_WRITE_ENABLE);
    SPI1_CS_HIGH();
}

static void W25Q128_WaitBusy(void)
{
    uint8_t reg;
    uint32_t timeout = 100000;
    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_READ_STATUS_REG1);
    do {
        reg = SPI1_ReadWriteByte(0xFF);
        if (--timeout == 0) break;
    } while (reg & 0x01);
    SPI1_CS_HIGH();
}

void W25Q128_Init(void)
{
    SPI1_Init();

    /* 发送复位命令 */
    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_ENABLE_RESET);
    SPI1_CS_HIGH();

    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_RESET);
    SPI1_CS_HIGH();

    W25Q128_WaitBusy();
}

uint32_t W25Q128_ReadID(void)
{
    uint32_t id = 0;

    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_READ_JEDEC_ID);
    id  = SPI1_ReadWriteByte(0xFF) << 16;  /* Manufacturer ID */
    id |= SPI1_ReadWriteByte(0xFF) << 8;   /* Memory Type */
    id |= SPI1_ReadWriteByte(0xFF);         /* Capacity */
    SPI1_CS_HIGH();

    return id;
}

void W25Q128_EraseSector(uint32_t sector_addr)
{
    sector_addr &= ~(W25Q_SECTOR_SIZE - 1);  /* 4KB对齐 */

    W25Q128_WriteEnable();
    W25Q128_WaitBusy();

    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_SECTOR_ERASE);
    SPI1_ReadWriteByte((sector_addr >> 16) & 0xFF);
    SPI1_ReadWriteByte((sector_addr >> 8) & 0xFF);
    SPI1_ReadWriteByte(sector_addr & 0xFF);
    SPI1_CS_HIGH();

    W25Q128_WaitBusy();
}

void W25Q128_EraseBlock64K(uint32_t block_addr)
{
    block_addr &= ~(W25Q_BLOCK64K_SIZE - 1);  /* 64KB对齐 */

    W25Q128_WriteEnable();
    W25Q128_WaitBusy();

    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_BLOCK64K_ERASE);
    SPI1_ReadWriteByte((block_addr >> 16) & 0xFF);
    SPI1_ReadWriteByte((block_addr >> 8) & 0xFF);
    SPI1_ReadWriteByte(block_addr & 0xFF);
    SPI1_CS_HIGH();

    W25Q128_WaitBusy();
}

void W25Q128_Write(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t page_remain;

    while (len > 0) {
        /* 计算当前页剩余空间 */
        page_remain = W25Q_PAGE_SIZE - (addr % W25Q_PAGE_SIZE);
        if (len < page_remain) {
            page_remain = len;
        }

        W25Q128_WriteEnable();
        W25Q128_WaitBusy();

        SPI1_CS_LOW();
        SPI1_ReadWriteByte(CMD_PAGE_PROGRAM);
        SPI1_ReadWriteByte((addr >> 16) & 0xFF);
        SPI1_ReadWriteByte((addr >> 8) & 0xFF);
        SPI1_ReadWriteByte(addr & 0xFF);

        for (uint32_t i = 0; i < page_remain; i++) {
            SPI1_ReadWriteByte(buf[i]);
        }
        SPI1_CS_HIGH();

        W25Q128_WaitBusy();

        addr += page_remain;
        buf  += page_remain;
        len  -= page_remain;
    }
}

void W25Q128_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    SPI1_CS_LOW();
    SPI1_ReadWriteByte(CMD_READ_DATA);
    SPI1_ReadWriteByte((addr >> 16) & 0xFF);
    SPI1_ReadWriteByte((addr >> 8) & 0xFF);
    SPI1_ReadWriteByte(addr & 0xFF);

    for (uint32_t i = 0; i < len; i++) {
        buf[i] = SPI1_ReadWriteByte(0xFF);
    }
    SPI1_CS_HIGH();
}
