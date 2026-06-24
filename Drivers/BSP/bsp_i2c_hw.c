#include "bsp_i2c_hw.h"

#define I2C_TIMEOUT  10000

static uint32_t I2C_Timeout;

static uint8_t I2C_WaitEvent(uint32_t event)
{
    I2C_Timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, event)) {
        if (--I2C_Timeout == 0) return 1;
    }
    return 0;
}

void HW_I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;

    /* 使能时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* PB6=SCL, PB7=SDA -> AF4 (I2C1) */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1配置: 400kHz */
    I2C_InitStruct.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1         = 0x00;
    I2C_InitStruct.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_ClockSpeed          = 400000;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
}

uint8_t HW_I2C1_WriteBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    /* 起始条件 */
    I2C_GenerateSTART(I2C1, ENABLE);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT)) return 1;

    /* 发送设备地址(写) */
    I2C_Send7bitAddress(I2C1, dev_addr, I2C_Direction_Transmitter);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) return 2;

    /* 发送寄存器地址 */
    I2C_SendData(I2C1, reg_addr);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return 3;

    /* 发送数据 */
    for (uint16_t i = 0; i < len; i++) {
        I2C_SendData(I2C1, data[i]);
        if (I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return 4;
    }

    /* 停止条件 */
    I2C_GenerateSTOP(I2C1, ENABLE);

    return 0;
}

uint8_t HW_I2C1_ReadBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    /* 起始条件 */
    I2C_GenerateSTART(I2C1, ENABLE);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT)) return 1;

    /* 发送设备地址(写) - 设置读取地址 */
    I2C_Send7bitAddress(I2C1, dev_addr, I2C_Direction_Transmitter);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) return 2;

    /* 发送寄存器地址 */
    I2C_SendData(I2C1, reg_addr);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return 3;

    /* 重复起始条件 */
    I2C_GenerateSTART(I2C1, ENABLE);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT)) return 4;

    /* 发送设备地址(读) */
    I2C_Send7bitAddress(I2C1, dev_addr, I2C_Direction_Receiver);
    if (I2C_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) return 5;

    /* 接收数据 */
    for (uint16_t i = 0; i < len; i++) {
        if (i == len - 1) {
            I2C_AcknowledgeConfig(I2C1, DISABLE);  /* 最后一个字节NACK */
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        if (I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)) return 6;
        data[i] = I2C_ReceiveData(I2C1);
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return 0;
}
