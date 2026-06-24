#include "stm32f4xx.h"
#include "bsp_oled.h"
#include "bsp_rtc.h"
#include "FreeRTOS.h"
#include "task.h"

// ����ʱ��״̬ö��
typedef enum {
    SET_RETURN = 0,
    SET_YEAR = 1,
    SET_MONTH = 2,
    SET_DAY = 3,
    SET_HOUR = 4,
    SET_MIN = 5,
    SET_SEC = 6
} SetTimeState_t;

static SetTimeState_t currentSetState = SET_RETURN;
static uint8_t editingValue = 0;  // �Ƿ����ڱ༭��ֵ

/**
  * @brief  ��ʾ�������ڽ���
  */
static void Show_SetDate_UI(void)
{
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_Printf(0, 16, OLED_8X16, "��:%4d", MyRTC_Time[0]);
    OLED_Printf(0, 32, OLED_8X16, "��:%2d", MyRTC_Time[1]);
    OLED_Printf(0, 48, OLED_8X16, "��:%2d", MyRTC_Time[2]);
}

/**
  * @brief  ��ʾ����ʱ�����
  */
static void Show_SetClock_UI(void)
{
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_Printf(0, 16, OLED_8X16, "ʱ:%2d", MyRTC_Time[3]);
    OLED_Printf(0, 32, OLED_8X16, "��:%2d", MyRTC_Time[4]);
    OLED_Printf(0, 48, OLED_8X16, "��:%2d", MyRTC_Time[5]);
}

/**
  * @brief  ����RTCʱ��ֵ
  * @param  index: ʱ������ (0-5�ֱ����������ʱ����)
  * @param  increment: 1=����, 0=����
  */
static void Adjust_RTC_Value(uint8_t index, uint8_t increment)
{
    int16_t value = MyRTC_Time[index];
    
    if(increment) {
        value++;
    } else {
        value--;
    }
    
    // �߽紦��
    switch(index) {
        case 0: // �� (2000-2099)
            if(value < 2000) value = 2099;
            if(value > 2099) value = 2000;
            break;
        case 1: // �� (1-12)
            if(value < 1) value = 12;
            if(value > 12) value = 1;
            break;
        case 2: // �� (1-31)
            if(value < 1) value = 31;
            if(value > 31) value = 1;
            break;
        case 3: // ʱ (0-23)
            if(value < 0) value = 23;
            if(value > 23) value = 0;
            break;
        case 4: // �� (0-59)
        case 5: // �� (0-59)
            if(value < 0) value = 59;
            if(value > 59) value = 0;
            break;
    }
    
    MyRTC_Time[index] = value;
    MyRTC_SetTime();
}

/**
  * @brief  ����ʱ�䰴������
  * @param  KeyNum: 1=��/��, 2=��/��, 3=ȷ��
  * @retval 0=��������, 1=�˳�����
  */
uint8_t SetTime_HandleKey(uint8_t KeyNum)
{
    if(editingValue) {
        // ���ڱ༭��ֵ
        if(KeyNum == 1) {
            // ��ֵ��
            Adjust_RTC_Value(currentSetState - 1, 0);
        }
        else if(KeyNum == 2) {
            // ��ֵ��
            Adjust_RTC_Value(currentSetState - 1, 1);
        }
        else if(KeyNum == 3) {
            // ȷ���޸�
            editingValue = 0;
        }
    }
    else {
        // ����ģʽ
        if(KeyNum == 1) {
            // ��һ��ѡ��
            if(currentSetState == SET_RETURN) {
                currentSetState = SET_SEC;
            } else {
                currentSetState--;
            }
        }
        else if(KeyNum == 2) {
            // ��һ��ѡ��
            if(currentSetState == SET_SEC) {
                currentSetState = SET_RETURN;
            } else {
                currentSetState++;
            }
        }
        else if(KeyNum == 3) {
            // ����༭�򷵻�
            if(currentSetState == SET_RETURN) {
                // ������һ��
                currentSetState = SET_RETURN;
                return 1;  // �˳�����
            } else {
                // ��ʼ�༭
                editingValue = 1;
            }
        }
    }
    
    return 0;  // ��������
}

/**
  * @brief  ˢ������ʱ�����
  */
void SetTime_RefreshDisplay(void)
{
    OLED_Clear();
    
    // ����״̬��ʾ��ͬ����
    if(currentSetState <= SET_DAY) {
        Show_SetDate_UI();
    } else {
        Show_SetClock_UI();
    }
    
    // ������ɫ��ʾ
    if(editingValue) {
        // �༭״̬�·�ɫ��ֵ
        switch(currentSetState) {
            case SET_YEAR:
                OLED_ReverseArea(24, 16, 32, 16);
                break;
            case SET_MONTH:
                OLED_ReverseArea(24, 32, 16, 16);
                break;
            case SET_DAY:
                OLED_ReverseArea(24, 48, 16, 16);
                break;
            case SET_HOUR:
                OLED_ReverseArea(24, 16, 16, 16);
                break;
            case SET_MIN:
                OLED_ReverseArea(24, 32, 16, 16);
                break;
            case SET_SEC:
                OLED_ReverseArea(24, 48, 16, 16);
                break;
            default:
                break;
        }
    } else {
        // ����״̬�·�ɫ��ǩ
        switch(currentSetState) {
            case SET_RETURN:
                if(currentSetState <= SET_DAY) {
                    OLED_ReverseArea(0, 0, 16, 16);
                } else {
                    OLED_ReverseArea(0, 0, 16, 16);
                }
                break;
            case SET_YEAR:
                OLED_ReverseArea(0, 16, 16, 16);
                break;
            case SET_MONTH:
                OLED_ReverseArea(0, 32, 16, 16);
                break;
            case SET_DAY:
                OLED_ReverseArea(0, 48, 16, 16);
                break;
            case SET_HOUR:
                OLED_ReverseArea(0, 16, 16, 16);
                break;
            case SET_MIN:
                OLED_ReverseArea(0, 32, 16, 16);
                break;
            case SET_SEC:
                OLED_ReverseArea(0, 48, 16, 16);
                break;
        }
    }
    
    OLED_Update();
}

/**
  * @brief  ��ʼ������ʱ��ģ��
  */
void SetTime_Init(void)
{
    currentSetState = SET_RETURN;
    editingValue = 0;
}
