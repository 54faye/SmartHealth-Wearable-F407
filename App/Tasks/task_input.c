#include "task_common.h"
#include "app_config.h"
#include "UI_Manager.h"
#include "bsp_key.h"

/**
  * @brief  按键处理任务 - 处理按键事件并切换页面
  * @param  argument: 未使用
  * @retval None
  */
void KeyProcessTask(void *argument)
{
    uint8_t KeyNum = 0;
    
    while(1)
    {
        /* 阻塞等待按键消息，超时100ms */
        if(xQueueReceive(KeyQueue, &KeyNum, pdMS_TO_TICKS(100)) == pdPASS)
        {
            /* 唤醒屏幕 */
            IsScreenOn = 1;
            LastActionTime = xTaskGetTickCount();

            /* 调用UI管理器处理按键 */
            UI_HandleKey(KeyNum);
        }
        
        vTaskDelay(pdMS_TO_TICKS(10)); // 10ms延时
    }
}

/**
  * @brief  发送按键消息到队列
  * @param  KeyNum: 按键值
  * @retval None
  */
void SendKeyToQueue(uint8_t KeyNum)
{
    if(KeyQueue != NULL)
    {
        xQueueSendFromISR(KeyQueue, &KeyNum, NULL);
    }
}
