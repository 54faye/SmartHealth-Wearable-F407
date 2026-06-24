#include "bsp_spi1.h"

void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;

    /* 使能时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    /* SCK/MISO/MOSI -> AF5 */
    GPIO_PinAFConfig(SPI1_SCK_GPIO, SPI1_SCK_SOURCE, GPIO_AF_SPI1);
    GPIO_PinAFConfig(SPI1_MISO_GPIO, SPI1_MISO_SOURCE, GPIO_AF_SPI1);
    GPIO_PinAFConfig(SPI1_MOSI_GPIO, SPI1_MOSI_SOURCE, GPIO_AF_SPI1);

    GPIO_InitStruct.GPIO_Pin   = SPI1_SCK_PIN | SPI1_MISO_PIN | SPI1_MOSI_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CS -> 推挽输出，默认高电平 */
    GPIO_InitStruct.GPIO_Pin   = SPI1_CS_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI1_CS_GPIO, &GPIO_InitStruct);

    SPI1_CS_HIGH();

    /* SPI1配置: APB2=84MHz, /16=5.25MHz, Mode0, 8bit, MSB先 */
    SPI_InitStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStruct.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_ReadWriteByte(uint8_t tx_data)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, tx_data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}
