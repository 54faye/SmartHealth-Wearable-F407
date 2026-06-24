#ifndef __BSP_RTC_H
#define __BSP_RTC_H

extern int MyRTC_Time[];

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

#endif
