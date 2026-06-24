#ifndef __BSP_USART3_H
#define __BSP_USART3_H

#include "stm32f4xx.h"

#define USART3_RX_BUF_SIZE  2048

void USART3_Init(uint32_t baud);
void USART3_SendByte(uint8_t ch);
void USART3_SendString(char *str);
void USART3_SendData(uint8_t *data, uint32_t len);
uint8_t USART3_ReceiveByte(void);
uint8_t USART3_ReceiveByteTimeout(uint32_t timeout_ms);
uint32_t USART3_GetRxCount(void);
uint8_t USART3_ReadByte(void);
void USART3_FlushRx(void);

#endif
