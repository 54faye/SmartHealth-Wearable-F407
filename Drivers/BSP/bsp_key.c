#include "bsp_key.h"

uint8_t Key_Num;

extern void SendKeyToQueue(uint8_t KeyNum);

void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* KEY1: PA0, 高电平有效, 下拉输入 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* KEY2: PA1, 低电平有效, 上拉输入 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* KEY3: PA4, 低电平有效, 上拉输入 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
    uint8_t temp;
    if (Key_Num)
    {
        temp = Key_Num;
        Key_Num = 0;
        return temp;
    }
    return 0;
}

uint8_t Key_GetState(void)
{
    /* KEY1: PA0, 高电平按下 */
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
        return 1;
    /* KEY2: PA1, 低电平按下 */
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
        return 2;
    /* KEY3: PA4, 低电平按下 */
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
        return 3;
    return 0;
}

void Key_Tick(void)
{
    static uint8_t Count;
    static uint8_t CurrentState, PreState;
    Count++;
    if (Count >= 20)
    {
        Count = 0;
        PreState = CurrentState;
        CurrentState = Key_GetState();
        if (PreState != 0 && CurrentState == 0)
        {
            Key_Num = PreState;
            SendKeyToQueue(Key_Num);
        }
    }
}
