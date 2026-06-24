#include "UI_Manager.h"
#include "Page_Clock.h"
#include "Page_Menu.h"
#include "Page_Apps.h"

/* 页面控制变量 */
static PageState_t CurrentPage = PAGE_CLOCK;
static PageState_t LastPage = PAGE_CLOCK;

void UI_Init(void)
{
    CurrentPage = PAGE_CLOCK;
}

PageState_t UI_GetCurrentPage(void)
{
    return CurrentPage;
}

void UI_SwitchPage(PageState_t page)
{
    LastPage = CurrentPage;
    CurrentPage = page;
    
    switch(page)
    {
        case PAGE_MENU:
            // Page_Menu_Reset(); // 注释掉此行，以保留菜单选中位置
            break;
        case PAGE_STOPWATCH:
            Page_StopWatch_Reset();
            break;
        case PAGE_LED:
            Page_LED_Reset();
            break;
        case PAGE_SETTING:
            Page_Setting_Reset();
            break;
        default:
            break;
    }
}

void UI_Refresh(void)
{
    switch(CurrentPage)
    {
        case PAGE_CLOCK:
            Page_Clock_Draw();
            break;
            
        case PAGE_MENU:
            Page_Menu_Draw();
            break;
            
        case PAGE_STOPWATCH:
            Page_StopWatch_Draw();
            break;
            
        case PAGE_LED:
            Page_LED_Draw();
            break;
            
        case PAGE_MPU6050:
            Page_MPU6050_Draw();
            break;
            
        case PAGE_GAME:
            Page_Game_Draw();
            break;
            
        case PAGE_EMOJI:
            Page_Emoji_Draw();
            break;
            
        case PAGE_GRADIENTER:
            Page_Gradienter_Draw();
            break;
            
        case PAGE_SETTING:
            Page_Setting_Draw();
            break;
            
        case PAGE_STEPS:
            Page_Steps_Draw();
            break;
            
        default:
            CurrentPage = PAGE_CLOCK;
            break;
    }
}

void UI_HandleKey(uint8_t KeyNum)
{
    switch(CurrentPage)
    {
        case PAGE_CLOCK:
            Page_Clock_HandleKey(KeyNum);
            break;
            
        case PAGE_MENU:
            Page_Menu_HandleKey(KeyNum);
            break;
            
        case PAGE_STOPWATCH:
            Page_StopWatch_HandleKey(KeyNum);
            break;
            
        case PAGE_LED:
            Page_LED_HandleKey(KeyNum);
            break;
            
        case PAGE_MPU6050:
            Page_MPU6050_HandleKey(KeyNum);
            break;
            
        case PAGE_GAME:
            Page_Game_HandleKey(KeyNum);
            break;
            
        case PAGE_EMOJI:
            Page_Emoji_HandleKey(KeyNum);
            break;
            
        case PAGE_GRADIENTER:
            Page_Gradienter_HandleKey(KeyNum);
            break;
            
        case PAGE_SETTING:
            Page_Setting_HandleKey(KeyNum);
            break;
            
        case PAGE_STEPS:
            Page_Steps_HandleKey(KeyNum);
            break;
    }
}


