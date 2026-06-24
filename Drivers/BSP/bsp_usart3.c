#include "bsp_usart3.h"

static uint8_t rx_buf[USART3_RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;  /* 写入位置(ISR) */
static volatile uint16_t rx_tail = 0;  /* 读取位置(主循环) */

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        uint16_t next = (rx_head + 1) % USART3_RX_BUF_SIZE;
        rx_buf[rx_head] = USART_ReceiveData(USART3);
        if (next != rx_tail) {
            rx_head = next;
        }
    }
}

void USART3_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* 使能时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* PB10=RX, PB11=TX -> AF7 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3配置 */
    USART_InitStruct.USART_BaudRate            = baud;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStruct);

    /* 使能RXNE中断 */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART3, ENABLE);
}

void USART3_SendByte(uint8_t ch)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, ch);
}

void USART3_SendString(char *str)
{
    while (*str) {
        USART3_SendByte(*str++);
    }
}

void USART3_SendData(uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        USART3_SendByte(data[i]);
    }
}

uint8_t USART3_ReceiveByte(void)
{
    while (rx_head == rx_tail);
    uint8_t ch = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % USART3_RX_BUF_SIZE;
    return ch;
}

uint8_t USART3_ReceiveByteTimeout(uint32_t timeout_ms)
{
    uint32_t start = 0;  /* 简单计数器 */
    while (rx_head == rx_tail) {
        for (volatile int i = 0; i < 5000; i++);
        if (++start > timeout_ms) return 0;
    }
    uint8_t ch = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % USART3_RX_BUF_SIZE;
    return ch;
}

uint32_t USART3_GetRxCount(void)
{
    if (rx_head >= rx_tail) return rx_head - rx_tail;
    return USART3_RX_BUF_SIZE - rx_tail + rx_head;
}

uint8_t USART3_ReadByte(void)
{
    uint8_t ch = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % USART3_RX_BUF_SIZE;
    return ch;
}

void USART3_FlushRx(void)
{
    rx_head = 0;
    rx_tail = 0;
}
