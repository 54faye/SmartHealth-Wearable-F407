#include "bsp_usart.h"

void USART1_Init(uint32_t baud)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    /* 使能时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* 引脚复用功能 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    /* 复位USART */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);

    /* TX: PA9 */
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* RX: PA10 */
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART配置 */
    USART_InitStruct.USART_BaudRate            = baud;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

/* printf重定向 */
#if 1
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);
    USART1->DR = (uint8_t)ch;
    return ch;
}
#endif
