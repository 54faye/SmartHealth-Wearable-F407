#include "stm32f4xx.h"
#include "math.h"
#include "pedometer.h"
#include "bsp_rtc.h"
#include "debug_serial.h"

// �Ʋ��㷨����
#define STEP_THRESHOLD 		2000  // ���ٶȱ仯��ֵ (��׼��16384)
#define STEP_MIN_INTERVAL 	300   // ��С�������(ms)

static uint32_t StepCount = 0;
static uint32_t LastStepTime = 0;
static float LastAccelMag = 0;
static uint8_t StepState = 0; // 0: �ȴ���ֵ, 1: �ȴ�����

void Pedometer_Init(void)
{
	// ��ȡ����Ĳ���������У�
	// ����򻯴�����ÿ���ϵ������㣬���ߴ�Flash��ȡ���һ����¼
	// ʵ��Ӧ����Ҫ����Flash�ҵ�����ļ�¼
	StepCount = 0;
}

uint32_t Pedometer_GetStepCount(void)
{
	return StepCount;
}

void Pedometer_Reset(void)
{
	StepCount = 0;
}

// ����MPU6050ԭʼ����
void Pedometer_Update(int16_t ax, int16_t ay, int16_t az, uint32_t current_tick)
{
	float accel_mag = sqrt(ax*ax + ay*ay + az*az);
	
	// �򵥵ķ�ֵ���
	// ʵ���������ٶ�ԼΪ 16384
	
	if (StepState == 0)
	{
		if (accel_mag > 16384 + STEP_THRESHOLD)
		{
			StepState = 1;
		}
	}
	else if (StepState == 1)
	{
		if (accel_mag < 16384)
		{
			if (current_tick - LastStepTime > STEP_MIN_INTERVAL)
			{
				StepCount++;
				LastStepTime = current_tick;
				Serial_Printf("Steps: %d\r\n", StepCount); // �������Ͳ���
			}
			StepState = 0;
		}
	}
}
