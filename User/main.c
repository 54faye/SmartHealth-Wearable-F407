#include "stm32f4xx.h"
#include "bsp_delay.h"
#include "bsp_oled.h"
#include "bsp_timer.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_rtc.h"
#include "bsp_mpu6050.h"
#include "bsp_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_manager.h"
#include "task_common.h"
#include "app_config.h"
#include "test_ota.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    SysTick_Init(168);
    USART1_Init(115200);
    printf("[DBG] System started\r\n");

    OLED_Init();
    printf("[DBG] OLED OK\r\n");

    /* OLED hardware test - fill screen white */
    {
        uint8_t i, j;
        for (j = 0; j < 8; j++)
            for (i = 0; i < 128; i++)
                OLED_DisplayBuf[j][i] = 0xFF;
        OLED_Update();
        printf("[DBG] OLED test pattern sent\r\n");
    }

    Key_Init();
    LED_Init();
    Timer_Init();
    MyRTC_Init();
    printf("[DBG] Peripherals OK\r\n");

    uint8_t mpu_status = MPU6050_Init();
    MPU6050_Available = (mpu_status == 0) ? 1 : 0;
    printf("[DBG] MPU6050 status=%d\r\n", mpu_status);

    xTaskCreate(StartTask, "StartTask", TASK_STACK_SIZE_START, NULL, TASK_PRIORITY_START, &StartTaskHandle);
    printf("[DBG] Starting scheduler\r\n");

    vTaskStartScheduler();

    for(;;);
}
