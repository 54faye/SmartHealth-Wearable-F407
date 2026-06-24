#include "DataCenter.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* 私有数据存储 */
static SensorData_t SensorData;
static StopWatchData_t StopWatchData;

/* 互斥量 */
static SemaphoreHandle_t SensorMutex = NULL;

void DataCenter_Init(void)
{
    /* 初始化数据 */
    SensorData.Roll = 0;
    SensorData.Pitch = 0;
    SensorData.Yaw = 0;
    
    StopWatchData.hour = 0;
    StopWatchData.min = 0;
    StopWatchData.sec = 0;
    StopWatchData.isRunning = 0;
    
    /* 创建互斥量 */
    SensorMutex = xSemaphoreCreateMutex();
}

/* ================= 传感器部分 ================= */

void DataCenter_SetSensorData(float roll, float pitch, float yaw)
{
    if(SensorMutex != NULL)
    {
        if(xSemaphoreTake(SensorMutex, portMAX_DELAY) == pdTRUE)
        {
            SensorData.Roll = roll;
            SensorData.Pitch = pitch;
            SensorData.Yaw = yaw;
            xSemaphoreGive(SensorMutex);
        }
    }
}

SensorData_t DataCenter_GetSensorData(void)
{
    SensorData_t data = {0};
    if(SensorMutex != NULL)
    {
        if(xSemaphoreTake(SensorMutex, portMAX_DELAY) == pdTRUE)
        {
            data = SensorData;
            xSemaphoreGive(SensorMutex);
        }
    }
    return data;
}

/* ================= 秒表部分 ================= */
/* 注意：Tick函数在中断中调用，不能使用互斥量 */

void DataCenter_StopWatch_Tick(void)
{
    static uint16_t Count = 0;
    
    Count++;
    if(Count >= 1000)
    {
        Count = 0;
        if(StopWatchData.isRunning)
        {
            StopWatchData.sec++;
            if(StopWatchData.sec >= 60)
            {
                StopWatchData.sec = 0;
                StopWatchData.min++;
                if(StopWatchData.min >= 60)
                {
                    StopWatchData.min = 0;
                    StopWatchData.hour++;
                    if(StopWatchData.hour > 99)
                    {
                        StopWatchData.hour = 0;
                    }
                }
            }
        }
    }
}

void DataCenter_StopWatch_Start(void)
{
    StopWatchData.isRunning = 1;
}

void DataCenter_StopWatch_Pause(void)
{
    StopWatchData.isRunning = 0;
}

void DataCenter_StopWatch_Reset(void)
{
    StopWatchData.isRunning = 0;
    StopWatchData.hour = 0;
    StopWatchData.min = 0;
    StopWatchData.sec = 0;
}

StopWatchData_t DataCenter_GetStopWatchData(void)
{
    /* 简单读取，暂不加锁，因为是单字节/简单结构读取 */
    return StopWatchData;
}
