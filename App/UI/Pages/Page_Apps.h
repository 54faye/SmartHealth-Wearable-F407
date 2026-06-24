#ifndef __PAGE_APPS_H
#define __PAGE_APPS_H

#include "stm32f4xx.h"

/* ��� */
void Page_StopWatch_Draw(void);
void Page_StopWatch_HandleKey(uint8_t KeyNum);
void Page_StopWatch_Reset(void);

/* LED */
void Page_LED_Draw(void);
void Page_LED_HandleKey(uint8_t KeyNum);
void Page_LED_Reset(void);

/* MPU6050 */
void Page_MPU6050_Draw(void);
void Page_MPU6050_HandleKey(uint8_t KeyNum);

/* ��Ϸ */
void Page_Game_Draw(void);
void Page_Game_HandleKey(uint8_t KeyNum);

/* ���� */
void Page_Emoji_Draw(void);
void Page_Emoji_HandleKey(uint8_t KeyNum);

/* ˮƽ�� */
void Page_Gradienter_Draw(void);
void Page_Gradienter_HandleKey(uint8_t KeyNum);

/* ���� */
void Page_Setting_Draw(void);
void Page_Setting_HandleKey(uint8_t KeyNum);
void Page_Setting_Reset(void);

/* �Ʋ��� */
void Page_Steps_Draw(void);
void Page_Steps_HandleKey(uint8_t KeyNum);

#endif
