#include "stm32f4xx.h"
#include "bsp_rtc.h"
#include <time.h>
#include <stdio.h>

int MyRTC_Time[] = {2025, 10, 20, 12, 20, 55};

void MyRTC_SetTime(void);


void MyRTC_Init(void)
{
    uint32_t timeout;
    uint8_t use_lsi = 0;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    if (RTC_ReadBackupRegister(RTC_BKP_DR1) != 0xA5A5)
    {
        printf("  RTC: first init, trying LSE...\r\n");

        /* 尝试启动LSE (外部32.768kHz晶振) */
        RCC_LSEConfig(RCC_LSE_ON);
        timeout = 500000;
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET && timeout--);

        if (timeout == 0)
        {
            printf("  RTC: LSE timeout, switching to LSI\r\n");
            /* LSE超时，改用LSI (内部~32kHz振荡器) */
            RCC_LSEConfig(RCC_LSE_OFF);
            RCC_LSICmd(ENABLE);
            timeout = 500000;
            while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET && timeout--);
            use_lsi = 1;
            if (timeout == 0) printf("  RTC: LSI also timeout!\r\n");
            else printf("  RTC: LSI ready\r\n");
        }
        else
        {
            printf("  RTC: LSE ready\r\n");
        }

        if (use_lsi)
        {
            RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
            RCC_RTCCLKCmd(ENABLE);
            RTC_WaitForSynchro();

            RTC_InitTypeDef RTC_InitStructure;
            RTC_InitStructure.RTC_AsynchPrediv = 127;   /* LSI ~32kHz: 32000/(128*250)=1Hz */
            RTC_InitStructure.RTC_SynchPrediv  = 249;
            RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
            RTC_Init(&RTC_InitStructure);
        }
        else
        {
            RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
            RCC_RTCCLKCmd(ENABLE);
            RTC_WaitForSynchro();

            RTC_InitTypeDef RTC_InitStructure;
            RTC_InitStructure.RTC_AsynchPrediv = 0x7F;   /* LSE 32768Hz: 32768/(128*256)=1Hz */
            RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
            RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
            RTC_Init(&RTC_InitStructure);
        }

        MyRTC_SetTime();

        RTC_WriteBackupRegister(RTC_BKP_DR1, 0xA5A5);
    }
    else
    {
        RTC_WaitForSynchro();
    }
}

void MyRTC_SetTime(void)
{
    time_t time_cnt;
    struct tm time_date;

    time_date.tm_year = MyRTC_Time[0] - 1900;
    time_date.tm_mon = MyRTC_Time[1] - 1;
    time_date.tm_mday = MyRTC_Time[2];
    time_date.tm_hour = MyRTC_Time[3];
    time_date.tm_min = MyRTC_Time[4];
    time_date.tm_sec = MyRTC_Time[5];

    time_cnt = mktime(&time_date);

    RTC_WriteBackupRegister(RTC_BKP_DR2, (uint32_t)time_cnt);

    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    time_cnt += 8 * 60 * 60;
    time_date = *localtime(&time_cnt);

    RTC_TimeStructure.RTC_Hours   = time_date.tm_hour;
    RTC_TimeStructure.RTC_Minutes = time_date.tm_min;
    RTC_TimeStructure.RTC_Seconds = time_date.tm_sec;
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

    RTC_DateStructure.RTC_Year    = time_date.tm_year - 100;
    RTC_DateStructure.RTC_Month   = time_date.tm_mon + 1;
    RTC_DateStructure.RTC_Date    = time_date.tm_mday;
    RTC_DateStructure.RTC_WeekDay = time_date.tm_wday + 1;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

void MyRTC_ReadTime(void)
{
    time_t time_cnt;
    struct tm time_date;

    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

    time_date.tm_year = RTC_DateStructure.RTC_Year + 100;
    time_date.tm_mon  = RTC_DateStructure.RTC_Month - 1;
    time_date.tm_mday = RTC_DateStructure.RTC_Date;
    time_date.tm_hour = RTC_TimeStructure.RTC_Hours;
    time_date.tm_min  = RTC_TimeStructure.RTC_Minutes;
    time_date.tm_sec  = RTC_TimeStructure.RTC_Seconds;

    time_cnt = mktime(&time_date);
    time_cnt -= 8 * 60 * 60;

    time_date = *localtime(&time_cnt);

    MyRTC_Time[0] = time_date.tm_year + 1900;
    MyRTC_Time[1] = time_date.tm_mon + 1;
    MyRTC_Time[2] = time_date.tm_mday;
    MyRTC_Time[3] = time_date.tm_hour;
    MyRTC_Time[4] = time_date.tm_min;
    MyRTC_Time[5] = time_date.tm_sec;
}
