#include "task_common.h"
#include "app_config.h"
#include "UI_Manager.h"
#include "bsp_oled.h"
#include "debug_serial.h"

/* ��Դ�������� */
volatile TickType_t LastActionTime = 0;
volatile uint8_t IsScreenOn = 1;

/**
  * @brief  ��ʾ���� - ����ˢ��OLED��Ļ
  * @param  argument: δʹ��
  * @retval None
  */
void DisplayTask(void *argument)
{
    printf("[DBG] DisplayTask running\r\n");
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(DISPLAY_REFRESH_PERIOD);
    xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        /* ����Զ�Ϩ�� */
        if (IsScreenOn && (xTaskGetTickCount() - LastActionTime > pdMS_TO_TICKS(OLED_AUTO_OFF_TIMEOUT * 1000))) {
            IsScreenOn = 0;
            if(xSemaphoreTake(OLEDMutex, portMAX_DELAY) == pdTRUE) {
                OLED_Clear();
                OLED_Update();
                xSemaphoreGive(OLEDMutex);
            }
        }

        if (!IsScreenOn) {
            vTaskDelay(pdMS_TO_TICKS(500)); // Ϩ��ʱ����ˢ����
            continue;
        }

        /* ��ȡOLED�����ź��� */
        if(xSemaphoreTake(OLEDMutex, portMAX_DELAY) == pdTRUE)
        {
            /* ����UI������ˢ�½��� */
            UI_Refresh();
            
            OLED_Update();
            xSemaphoreGive(OLEDMutex); // �ͷŻ����ź���
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency); // ��ȷ��ʱ
    }
}
