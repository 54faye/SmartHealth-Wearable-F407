#include "Page_Menu.h"
#include "UI_Manager.h"
#include "bsp_oled.h"
#include "dino.h"

static uint8_t MenuIndex = 1;
static uint8_t pre_selsection = 0;
static uint8_t target_selection = 0;
static uint8_t x_pre = 48;
static uint8_t move_flag = 0;
static uint8_t last_menu_index = 0;

void Page_Menu_Reset(void)
{
    MenuIndex = 1;
    pre_selsection = 0;
    target_selection = 0;
    x_pre = 48;
    move_flag = 0;
    last_menu_index = 0;
}

void Page_Menu_Draw(void)
{
    // 检测菜单索引变化
    if(last_menu_index != MenuIndex)
    {
        move_flag = 1;
        target_selection = (MenuIndex == 1) ? 0 : (MenuIndex - 1);
        last_menu_index = MenuIndex;
    }
    
    OLED_Clear();
    OLED_ShowImage(42, 10, 44, 44, Frame);
    
    // 处理图标移动动画
    if(pre_selsection < target_selection && move_flag)
    {
        x_pre = (x_pre >= 4) ? (x_pre - 4) : 0;
        if(x_pre == 0)
        {
            pre_selsection++;
            move_flag = 0;
            x_pre = 48;
        }
    }
    else if(pre_selsection > target_selection && move_flag)
    {
        x_pre = (x_pre <= 92) ? (x_pre + 4) : 96;
        if(x_pre == 96)
        {
            pre_selsection--;
            move_flag = 0;
            x_pre = 48;
        }
    }
    
    // 显示菜单图标
    if(pre_selsection >= 1)
    {
        OLED_ShowImage(x_pre - 48, 16, 32, 32, Menu_Group[(pre_selsection - 1) % 8]);
    }
    if(pre_selsection >= 2)
    {
        OLED_ShowImage(x_pre - 96, 16, 32, 32, Menu_Group[(pre_selsection - 2) % 8]);
    }
    OLED_ShowImage(x_pre, 16, 32, 32, Menu_Group[pre_selsection % 8]);
    OLED_ShowImage(x_pre + 48, 16, 32, 32, Menu_Group[(pre_selsection + 1) % 8]);
    OLED_ShowImage(x_pre + 96, 16, 32, 32, Menu_Group[(pre_selsection + 2) % 8]);
}

void Page_Menu_HandleKey(uint8_t KeyNum)
{
    if(KeyNum == 1) // 上一个
    {
        MenuIndex = (MenuIndex == 1) ? 8 : (MenuIndex - 1);
    }
    else if(KeyNum == 2) // 下一个
    {
        MenuIndex = (MenuIndex == 8) ? 1 : (MenuIndex + 1);
    }
    else if(KeyNum == 3) // 确认
    {
        switch(MenuIndex)
        {
            case 1: // 返回时钟
                UI_SwitchPage(PAGE_CLOCK);
                break;
            case 2: // 秒表
                UI_SwitchPage(PAGE_STOPWATCH);
                break;
            case 3: // LED控制
                UI_SwitchPage(PAGE_LED);
                break;
            case 4: // MPU6050
                UI_SwitchPage(PAGE_MPU6050);
                break;
            case 5: // 游戏
                UI_SwitchPage(PAGE_GAME);
                DinoGame_Pos_Init();
                break;
            case 6: // 表情
                UI_SwitchPage(PAGE_EMOJI);
                break;
            case 7: // 水平仪
                UI_SwitchPage(PAGE_GRADIENTER);
                break;
            case 8: // 计步器
                UI_SwitchPage(PAGE_STEPS);
                break;
        }
    }
}
