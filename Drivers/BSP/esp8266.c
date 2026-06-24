#include "esp8266.h"
#include "bsp_usart3.h"
#include "bsp_delay.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ESP_BUF_SIZE  512

static char esp_buf[ESP_BUF_SIZE];
static volatile uint16_t esp_idx = 0;

/* 从UART3接收缓冲区中查找字符串 */
static uint8_t esp_wait_for(const char *target, uint32_t timeout_ms)
{
    uint32_t tick = 0;
    esp_idx = 0;
    memset(esp_buf, 0, sizeof(esp_buf));

    while (tick < timeout_ms) {
        while (USART3_GetRxCount() > 0) {
            char ch = USART3_ReadByte();
            if (esp_idx < ESP_BUF_SIZE - 1) {
                esp_buf[esp_idx++] = ch;
                esp_buf[esp_idx] = '\0';
            }
            if (strstr(esp_buf, target) != NULL) {
                return 0;
            }
            if (strstr(esp_buf, "ERROR") != NULL) {
                return 1;
            }
            if (strstr(esp_buf, "FAIL") != NULL) {
                return 2;
            }
        }
        Delay_ms(1);
        tick++;
    }
    return 3;
}

/* 发送AT指令并等待响应 */
static uint8_t esp_send_cmd(const char *cmd, const char *ack, uint32_t timeout_ms)
{
    USART3_FlushRx();
    USART3_SendString((char *)cmd);
    USART3_SendString("\r\n");
    return esp_wait_for(ack, timeout_ms);
}

void ESP8266_Init(void)
{
    USART3_Init(115200);
    Delay_ms(500);

    esp_send_cmd("AT", "OK", 1000);
    esp_send_cmd("ATE0", "OK", 500);
    esp_send_cmd("AT+CIPMUX=0", "OK", 500);
}

uint8_t ESP8266_ConnectWiFi(char *ssid, char *password)
{
    char cmd[128];

    if (esp_send_cmd("AT+CWMODE=1", "OK", 1000) != 0) return 1;
    Delay_ms(200);

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    if (esp_send_cmd(cmd, "OK", 15000) != 0) return 2;

    return 0;
}

uint8_t ESP8266_ConnectTCP(char *ip, uint16_t port)
{
    char cmd[128];

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d", ip, port);
    if (esp_send_cmd(cmd, "CONNECT", 10000) != 0) return 1;

    return 0;
}

uint8_t ESP8266_SendData(uint8_t *data, uint16_t len)
{
    char cmd[32];

    sprintf(cmd, "AT+CIPSEND=%d", len);
    USART3_FlushRx();
    USART3_SendString(cmd);
    USART3_SendString("\r\n");

    if (esp_wait_for(">", 3000) != 0) return 1;

    USART3_SendData(data, len);

    if (esp_wait_for("SEND OK", 5000) != 0) return 2;

    /* 清空缓冲区中残留的响应数据，确保后续接收从干净状态开始 */
    USART3_FlushRx();

    return 0;
}

void ESP8266_CloseTCP(void)
{
    esp_send_cmd("AT+CIPCLOSE", "OK", 2000);
}

/*
 * 流式接收HTTP响应
 * 解析ESP8266的 +IPD,<len>:<data> 协议
 * 跳过HTTP header，只将body数据写入buf
 * 返回接收到的body字节数
 */
uint32_t ESP8266_ReceiveHTTP(uint8_t *buf, uint32_t max_len, uint32_t timeout_ms)
{
    uint32_t total_body = 0;
    uint32_t tick = 0;

    /* 状态机变量 */
    enum { WAIT_PLUS, READ_IPD_LEN, READ_IPD_DATA } state = WAIT_PLUS;
    uint8_t crlf_count = 0;
    uint8_t header_done = 0;
    uint32_t ipd_remain = 0;
    char len_str[8];
    uint8_t len_idx = 0;

    while (tick < timeout_ms) {
        if (USART3_GetRxCount() > 0) {
            uint8_t ch = USART3_ReadByte();
            tick = 0;

            switch (state) {
            case WAIT_PLUS:
                /* 等待 "+IPD," 的 '+' 字符 */
                if (ch == '+') {
                    len_idx = 0;
                    state = READ_IPD_LEN;
                }
                break;

            case READ_IPD_LEN:
                /* 解析 "IPD,<len>:" 中的长度部分 */
                if (ch == ':') {
                    len_str[len_idx] = '\0';
                    ipd_remain = (uint32_t)atoi(len_str);
                    state = READ_IPD_DATA;
                } else if (ch >= '0' && ch <= '9') {
                    if (len_idx < 7) {
                        len_str[len_idx++] = ch;
                    }
                }
                /* 跳过 "IPD," 等非数字非':'字符 */
                break;

            case READ_IPD_DATA:
                /* 读取IPD数据块 */
                ipd_remain--;

                if (!header_done) {
                    /* HTTP header阶段：寻找 \r\n\r\n */
                    if (ch == '\r' || ch == '\n') {
                        crlf_count++;
                        if (crlf_count >= 4) {
                            header_done = 1;
                        }
                    } else {
                        crlf_count = 0;
                    }
                } else {
                    /* HTTP body数据 */
                    if (total_body < max_len) {
                        buf[total_body++] = ch;
                    }
                }

                /* 当前IPD块读取完毕 */
                if (ipd_remain == 0) {
                    state = WAIT_PLUS;
                }
                break;
            }
        } else {
            Delay_ms(1);
            tick++;

            /* 检测连接关闭 */
            if (esp_idx > 0) {
                esp_buf[esp_idx] = '\0';
                if (strstr(esp_buf, "CLOSED") != NULL) {
                    break;
                }
            }
        }
    }

    return total_body;
}
