#ifndef __UI_MANAGER_H
#define __UI_MANAGER_H

#include "stm32f4xx.h"

/* ҳ��״̬ö�� */
typedef enum {
    PAGE_CLOCK,         // ʱ��ҳ��
    PAGE_MENU,          // �˵�ҳ��
    PAGE_STOPWATCH,     // ���ҳ��
    PAGE_LED,           // LED����ҳ��
    PAGE_MPU6050,       // ��̬��ʾҳ��
    PAGE_GAME,          // ������Ϸҳ��
    PAGE_EMOJI,         // ����ҳ��
    PAGE_GRADIENTER,    // ˮƽ��ҳ��
    PAGE_SETTING,       // ����ҳ��
    PAGE_STEPS          // �Ʋ���ҳ��
} PageState_t;

void UI_Init(void);
void UI_Refresh(void);
void UI_HandleKey(uint8_t KeyNum);
PageState_t UI_GetCurrentPage(void);
void UI_SwitchPage(PageState_t page); // ����������ҳ�������л�

#endif
