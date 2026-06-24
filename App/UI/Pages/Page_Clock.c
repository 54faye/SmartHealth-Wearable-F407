#include "Page_Clock.h"
#include "UI_Manager.h"
#include "bsp_oled.h"
#include "bsp_rtc.h"

static uint8_t ClockIndex = 1;
extern int MyRTC_Time[];

void Page_Clock_Draw(void)
{
    OLED_Clear();
    MyRTC_ReadTime();
    OLED_Printf(0, 0, OLED_6X8, "%d-%d-%d", MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2]);
    OLED_Printf(16, 16, OLED_12X24, "%02d-%02d-%02d", MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]);
    OLED_ShowString(0, 48, "菜单", OLED_8X16);
    OLED_ShowString(96, 48, "设置", OLED_8X16);
    
    if(ClockIndex == 1)
    {
        OLED_ReverseArea(0, 48, 32, 16);
    }
    else
    {
        OLED_ReverseArea(96, 48, 32, 16);
    }
}

void Page_Clock_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 1) // 左选
    {
        ClockIndex = (ClockIndex == 1) ? 2 : 1;
    }
    else if(KeyNum == 2) // 右选
    {
        ClockIndex = (ClockIndex == 2) ? 1 : 2;
    }
    else if(KeyNum == 3) // 确认
    {
        if(ClockIndex == 1) // 进入菜单
        {
            UI_SwitchPage(PAGE_MENU);
        }
        else if(ClockIndex == 2) // 进入设置
        {
            UI_SwitchPage(PAGE_SETTING);
        }
    }
}
