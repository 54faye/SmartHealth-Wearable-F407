#include "bsp_at24c02.h"
#include "bsp_i2c_hw.h"
#include "bsp_delay.h"

void AT24C02_Init(void)
{
    HW_I2C1_Init();
}

void AT24C02_WriteByte(uint8_t addr, uint8_t data)
{
    HW_I2C1_WriteBytes(AT24C02_ADDR_WRITE, addr, &data, 1);
    Delay_ms(5);  /* 等待EEPROM写周期 */
}

uint8_t AT24C02_ReadByte(uint8_t addr)
{
    uint8_t data;
    HW_I2C1_ReadBytes(AT24C02_ADDR_WRITE, addr, &data, 1);
    return data;
}

void AT24C02_Write(uint8_t addr, uint8_t *data, uint8_t len)
{
    /*
     * AT24C02页大小8字节，不能跨页写入
     * 逐字节写入，每字节后等待5ms
     */
    for (uint8_t i = 0; i < len; i++) {
        AT24C02_WriteByte(addr + i, data[i]);
    }
}

void AT24C02_Read(uint8_t addr, uint8_t *data, uint8_t len)
{
    HW_I2C1_ReadBytes(AT24C02_ADDR_WRITE, addr, data, len);
}

void AT24C02_WriteBootFlags(BootFlags_t *flags)
{
    AT24C02_Write(0, (uint8_t *)flags, sizeof(BootFlags_t));
}

void AT24C02_ReadBootFlags(BootFlags_t *flags)
{
    AT24C02_Read(0, (uint8_t *)flags, sizeof(BootFlags_t));
}

void AT24C02_ClearOTAPending(void)
{
    BootFlags_t flags;
    AT24C02_ReadBootFlags(&flags);
    flags.ota_pending = 0;
    AT24C02_WriteBootFlags(&flags);
}
