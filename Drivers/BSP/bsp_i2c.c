#include "bsp_i2c.h"
#include "bsp_delay.h"

/* MPU6050 软件I2C: PB12(SCL), PB13(SDA) */
#define I2C_SCL_PORT   GPIOB
#define I2C_SCL_PIN    GPIO_Pin_12
#define I2C_SDA_PORT   GPIOB
#define I2C_SDA_PIN    GPIO_Pin_13

static void MyI2C_W_SCL(uint8_t BitValue)
{
    GPIO_WriteBit(I2C_SCL_PORT, I2C_SCL_PIN, (BitAction)BitValue);
    Delay_us(10);
}

static void MyI2C_W_SDA(uint8_t BitValue)
{
    GPIO_WriteBit(I2C_SDA_PORT, I2C_SDA_PIN, (BitAction)BitValue);
    Delay_us(10);
}

static uint8_t MyI2C_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(I2C_SDA_PORT, I2C_SDA_PIN);
    Delay_us(10);
    return BitValue;
}

void MyI2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(I2C_SCL_PORT, I2C_SCL_PIN);
    GPIO_SetBits(I2C_SDA_PORT, I2C_SDA_PIN);
}

void MyI2C_Start(void)
{
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SDA(!!(Byte & (0x80 >> i)));
        MyI2C_W_SCL(1);
        MyI2C_W_SCL(0);
    }
}

uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    MyI2C_W_SDA(1);
    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SCL(1);
        if (MyI2C_R_SDA()) { Byte |= (0x80 >> i); }
        MyI2C_W_SCL(0);
    }
    return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{
    MyI2C_W_SDA(AckBit);
    MyI2C_W_SCL(1);
    MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
    uint8_t AckBit;
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
    AckBit = MyI2C_R_SDA();
    MyI2C_W_SCL(0);
    return AckBit;
}
