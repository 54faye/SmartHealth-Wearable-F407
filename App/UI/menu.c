#if 0
#include "stm32f4xx.h"
#include "bsp_oled.h"
#include "bsp_rtc.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "SetTime.h"
#include "menu.h"
#include "bsp_mpu6050.h"
#include "bsp_delay.h"
#include "math.h"
#include "dino.h"

uint8_t KeyNum;
int clkflag = 1;
// ...existing code...
int Gradienter(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return 0;
		}
		OLED_Clear();
		Show_Gradienter_UI();
		OLED_Update();
	}
}
#endif


