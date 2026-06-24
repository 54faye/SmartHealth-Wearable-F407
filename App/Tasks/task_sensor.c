#include "task_common.h"
#include "app_config.h"
#include "DataCenter.h"
#include "bsp_mpu6050.h"
#include "kalman.h"
#include "step_algo.h"
#include "UI_Manager.h"
#include <math.h>

/* MPU6050状态 */
uint8_t MPU6050_Available = 0;

/* 局部变量定义 */
static int16_t ax, ay, az, gx, gy, gz;
static float Roll, Pitch, Yaw;

/* 卡尔曼滤波对象 */
static Kalman_t K_Roll;
static Kalman_t K_Pitch;

/**
  * @brief  MPU6050任务 - 读取传感器数据并解算姿态
  * @param  argument: 未使用
  * @retval None
  */
void MPU6050Task(void *argument)
{
    float roll_a, pitch_a;
    float Delta_t = 0.005f;
    const float pi = 3.14159265f;
    TickType_t samplePeriod = pdMS_TO_TICKS(MPU6050_SAMPLE_PERIOD_FAST);
    static float last_Roll = 0, last_Pitch = 0;
    static uint8_t last_posture_ok = 0; // 记录上一次姿态是否满足条件
    
    /* 初始化卡尔曼滤波器 */
    Kalman_Init(&K_Roll);
    Kalman_Init(&K_Pitch);

    Roll = 0;
    Pitch = 0;
    Yaw = 0;
    
    while(1)
    {
        if(MPU6050_Available) {
            /* MPU6050可用，读取数据 */
            MPU6050_GetData(&ax, &ay, &az, &gx, &gy, &gz);
            
            /* 更新计步器 */
            StepCounter_Update(ax, ay, az);
        } else {
            /* MPU6050不可用，使用默认值 */
            ax = ay = az = 0;
            gx = gy = gz = 0;
        }
        
        /* 计算加速度计角度 */
        pitch_a = atan2f((-1) * ax, az) * 180.0f / pi;
        roll_a = atan2f((-1) * ay, az) * 180.0f / pi;
        
        /* 卡尔曼滤波融合 (陀螺仪灵敏度 16.4 LSB/dps) */
        Roll = Kalman_GetAngle(&K_Roll, roll_a, gx / 16.4f, Delta_t);
        Pitch = Kalman_GetAngle(&K_Pitch, pitch_a, gy / 16.4f, Delta_t);
        Yaw += (gz / 16.4f) * Delta_t; // Yaw简单积分
        
        /* 更新数据中心 */
        DataCenter_SetSensorData(Roll, Pitch, Yaw);
        
        /* 运动检测 (用于自动熄屏) */
        if (fabs(Roll - last_Roll) > MOVEMENT_THRESHOLD || fabs(Pitch - last_Pitch) > MOVEMENT_THRESHOLD) {
            LastActionTime = xTaskGetTickCount();
        }
        last_Roll = Roll;
        last_Pitch = Pitch;
        
        /* 抬手亮屏检测 (边沿触发) */
        if (!IsScreenOn) {
            uint8_t current_posture_ok = 0;
            if (Roll >= WAKE_UP_ANGLE_ROLL_MIN && Roll <= WAKE_UP_ANGLE_ROLL_MAX &&
                Pitch >= WAKE_UP_ANGLE_PITCH_MIN && Pitch <= WAKE_UP_ANGLE_PITCH_MAX) {
                current_posture_ok = 1;
            }

            // 只有当姿态从“不满足”变为“满足”时才触发唤醒
            if (current_posture_ok && !last_posture_ok) {
                IsScreenOn = 1;
                LastActionTime = xTaskGetTickCount();
            }
            last_posture_ok = current_posture_ok;
        } else {
            // 屏幕亮时，实时更新姿态状态，防止熄屏瞬间误判
            if (Roll >= WAKE_UP_ANGLE_ROLL_MIN && Roll <= WAKE_UP_ANGLE_ROLL_MAX &&
                Pitch >= WAKE_UP_ANGLE_PITCH_MIN && Pitch <= WAKE_UP_ANGLE_PITCH_MAX) {
                last_posture_ok = 1;
            } else {
                last_posture_ok = 0;
            }
        }
        
        /* 根据当前页面动态调整采样率以节省功耗 */
        if(IsScreenOn && (UI_GetCurrentPage() == PAGE_MPU6050 || UI_GetCurrentPage() == PAGE_GRADIENTER))
        {
            samplePeriod = pdMS_TO_TICKS(MPU6050_SAMPLE_PERIOD_FAST);
        }
        else
        {
            samplePeriod = pdMS_TO_TICKS(MPU6050_SAMPLE_PERIOD_SLOW);
        }
        
        vTaskDelay(samplePeriod);
    }
}
