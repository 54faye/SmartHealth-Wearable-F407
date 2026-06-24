#ifndef __DATA_CENTER_H
#define __DATA_CENTER_H

#include "stm32f4xx.h"

/* ���������ݽṹ�� */
typedef struct {
    float Roll;
    float Pitch;
    float Yaw;
} SensorData_t;

/* ������ݽṹ�� */
typedef struct {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t isRunning;
} StopWatchData_t;

/* ��ʼ�� */
void DataCenter_Init(void);

/* ���������ݷ��� */
void DataCenter_SetSensorData(float roll, float pitch, float yaw);
SensorData_t DataCenter_GetSensorData(void);

/* ������������ */
void DataCenter_StopWatch_Tick(void); // ����ʱ���жϵ���
void DataCenter_StopWatch_Start(void);
void DataCenter_StopWatch_Pause(void);
void DataCenter_StopWatch_Reset(void);
StopWatchData_t DataCenter_GetStopWatchData(void);

#endif
