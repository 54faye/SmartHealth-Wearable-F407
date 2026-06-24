#include "Page_Apps.h"
#include "UI_Manager.h"
#include "bsp_oled.h"
#include "DataCenter.h"
#include "bsp_led.h"
#include "dino.h"
#include "SetTime.h"
#include "step_algo.h"
#include "FreeRTOS.h"
#include "task.h"

/* ================= 秒表 ================= */
static uint8_t StopWatchIndex = 1;

void Page_StopWatch_Reset(void)
{
    StopWatchIndex = 1;
}

void Page_StopWatch_Draw(void)
{
    StopWatchData_t sw = DataCenter_GetStopWatchData();
    OLED_Clear();
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_Printf(32, 20, OLED_8X16, "%02d:%02d:%02d", sw.hour, sw.min, sw.sec);
    OLED_ShowString(8, 44, "开始", OLED_8X16);
    OLED_ShowString(48, 44,"停止", OLED_8X16);
    OLED_ShowString(88, 44,"清除", OLED_8X16);
    
    switch(StopWatchIndex)
    {
        case 1: OLED_ReverseArea(0, 0, 16, 16); break;
        case 2: OLED_ReverseArea(8, 44, 32, 16); break;
        case 3: OLED_ReverseArea(48, 44, 32, 16); break;
        case 4: OLED_ReverseArea(88, 44, 32, 16); break;
    }
}

void Page_StopWatch_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 1) // 上/左
    {
        StopWatchIndex = (StopWatchIndex == 1) ? 4 : (StopWatchIndex - 1);
    }
    else if(KeyNum == 2) // 下/右
    {
        StopWatchIndex = (StopWatchIndex == 4) ? 1 : (StopWatchIndex + 1);
    }
    else if(KeyNum == 3) // 确认
    {
        switch(StopWatchIndex)
        {
            case 1: UI_SwitchPage(PAGE_MENU); break;
            case 2: DataCenter_StopWatch_Start(); break;
            case 3: DataCenter_StopWatch_Pause(); break;
            case 4: DataCenter_StopWatch_Reset(); break;
        }
    }
}

/* ================= LED ================= */
static uint8_t LEDIndex = 1;

void Page_LED_Reset(void)
{
    LEDIndex = 1;
}

void Page_LED_Draw(void)
{
    OLED_Clear();
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_ShowString(20, 20, "OFF", OLED_12X24);
    OLED_ShowString(70, 20, "ON", OLED_12X24);
    
    switch(LEDIndex)
    {
        case 1: OLED_ReverseArea(0, 0, 16, 16); break;
        case 2: OLED_ReverseArea(20, 20, 36, 24); break;
        case 3: OLED_ReverseArea(70, 20, 24, 24); break;
    }
}

void Page_LED_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 1) // 上/左
    {
        LEDIndex = (LEDIndex == 1) ? 3 : (LEDIndex - 1);
    }
    else if(KeyNum == 2) // 下/右
    {
        LEDIndex = (LEDIndex == 3) ? 1 : (LEDIndex + 1);
    }
    else if(KeyNum == 3) // 确认
    {
        if(LEDIndex == 1) UI_SwitchPage(PAGE_MENU);
        else if(LEDIndex == 2) LED1_OFF();
        else if(LEDIndex == 3) LED1_ON();
    }
}

/* ================= MPU6050 ================= */
void Page_MPU6050_Draw(void)
{
    SensorData_t sensor = DataCenter_GetSensorData();
    OLED_Clear();
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_Printf(0, 16, OLED_8X16, "Roll: %.1f", sensor.Roll);
    OLED_Printf(0, 32, OLED_8X16, "Pitch:%.1f", sensor.Pitch);
    OLED_Printf(0, 48, OLED_8X16, "Yaw:  %.1f", sensor.Yaw);
    OLED_ReverseArea(0, 0, 16, 16);
}

void Page_MPU6050_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 3) UI_SwitchPage(PAGE_MENU);
}

/* ================= 游戏 ================= */
void Page_Game_Draw(void)
{
    extern int Score;
    extern Object_Position barrier;
    extern Object_Position dino;
    
    OLED_Clear();
    OLED_ShowNum(98, 0, Score, 5, OLED_6X8);
    Show_Groud();
    Show_Barrier();
    Show_Cloud();
    Show_Dino();
    
    if(isColliding(&dino, &barrier))
    {
        UI_SwitchPage(PAGE_MENU);
    }
    Score_Tick(); // Update game state
}

void Page_Game_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 1) { Dino_Jump(); }
    else if(KeyNum == 3) UI_SwitchPage(PAGE_MENU);
}

/* ================= 表情 ================= */
void Page_Emoji_Draw(void)
{
    static uint8_t emoji_state = 0;
    static TickType_t last_time = 0;
    TickType_t current_time = xTaskGetTickCount();
    
    if(current_time - last_time >= pdMS_TO_TICKS(100))
    {
        emoji_state = (emoji_state + 1) % 6;
        last_time = current_time;
    }
    
    OLED_Clear();
    if(emoji_state < 3)
    {
        OLED_ShowImage(30, 10 + emoji_state, 16, 16, Eye_Brow[0]);
        OLED_ShowImage(82, 10 + emoji_state, 16, 16, Eye_Brow[1]);
        OLED_DrawEllipse(40, 32, 6, 6 - emoji_state, 1);
        OLED_DrawEllipse(88, 32, 6, 6 - emoji_state, 1);
    }
    else
    {
        uint8_t i = emoji_state - 3;
        OLED_ShowImage(30, 12 - i, 16, 16, Eye_Brow[0]);
        OLED_ShowImage(82, 12 - i, 16, 16, Eye_Brow[1]);
        OLED_DrawEllipse(40, 32, 6, 4 + i, 1);
        OLED_DrawEllipse(88, 32, 6, 4 + i, 1);
    }
    OLED_ShowImage(52, 40, 20, 20, Mouth);
}

void Page_Emoji_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 3) UI_SwitchPage(PAGE_MENU);
}

/* ================= 水平仪 ================= */
void Page_Gradienter_Draw(void)
{
    SensorData_t sensor = DataCenter_GetSensorData();
    OLED_Clear();
    OLED_DrawCircle(64, 32, 30, 0);
    OLED_DrawCircle(64 - (int8_t)sensor.Roll, 32 + (int8_t)sensor.Pitch, 4, 1);
}

void Page_Gradienter_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 3) UI_SwitchPage(PAGE_MENU);
}

/* ================= 设置 ================= */
static uint8_t InSettingMode = 0;

void Page_Setting_Reset(void)
{
    InSettingMode = 0;
}

void Page_Setting_Draw(void)
{
    if(InSettingMode) {
        SetTime_RefreshDisplay();
    } else {
        OLED_Clear();
        OLED_ShowImage(0, 0, 16, 16, Return);
        OLED_ShowString(0, 16, "设置日期时间", OLED_8X16);
        OLED_ReverseArea(0, 0, 16, 16);
    }
}

void Page_Setting_HandleKey(uint8_t KeyNum)
{
    if(!InSettingMode) {
        SetTime_Init();
        InSettingMode = 1;
    }
    
    if(SetTime_HandleKey(KeyNum)) {
        UI_SwitchPage(PAGE_CLOCK);
        InSettingMode = 0;
    }
}

/* ================= 计步器 ================= */
void Page_Steps_Draw(void)
{
    OLED_Clear();
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_ShowString(20, 0, "\xBD\xF1\xC8\xD5\xB2\xBD\xCA\xFD", OLED_8X16);
    OLED_ShowNum(20, 24, StepCounter_GetTodaySteps(), 5, OLED_12X24);
    OLED_ShowString(20, 50, "Total:", OLED_6X8);
    OLED_ShowNum(60, 50, StepCounter_GetTotalSteps(), 6, OLED_6X8);
    OLED_ReverseArea(0, 0, 16, 16);
}

void Page_Steps_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 3) UI_SwitchPage(PAGE_MENU);
}
