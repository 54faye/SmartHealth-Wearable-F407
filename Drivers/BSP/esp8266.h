#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f4xx.h"

#define ESP8266_BUF_SIZE  1500

void ESP8266_Init(void);
uint8_t ESP8266_ConnectWiFi(char *ssid, char *password);
uint8_t ESP8266_ConnectTCP(char *ip, uint16_t port);
uint8_t ESP8266_SendData(uint8_t *data, uint16_t len);
void ESP8266_CloseTCP(void);

/* 接收HTTP响应，解析+IPD，将body写入回调函数
 * 返回接收到的body总字节数，0表示失败 */
uint32_t ESP8266_ReceiveHTTP(uint8_t *buf, uint32_t max_len, uint32_t timeout_ms);

#endif
