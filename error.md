# 编译状态记录

## 当前状态：编译通过，调度器正常运行

**目标**: SmartWatch_F407
**编译器**: ARM Compiler V5.06 update 5 (build 528)
**预定义宏**: `USE_STDPERIPH_DRIVER, STM32F40_41xxx`
**FreeRTOS**: 官方 RVDS/ARM_CM4F 端口（含 FPU 支持）

### 编译结果
- **Error**: 0
- **Warning**: 3（均为无害警告）

### 剩余警告列表

| 文件 | 警告 | 说明 |
|------|------|------|
| `App/UI/UI_Manager.c` | #550-D: variable "LastPage" was set but never used | 预留的状态变量，暂未使用 |
| `App/UI/dino.c` | #1-D: last line of file ends without a newline | 文件末尾缺少换行符 |
| `Middlewares/Algorithm/pedometer.c` | #177-D: variable "LastAccelMag" was declared but never referenced | 预留的算法变量，暂未使用 |

---

## 已解决的编译/运行错误

### 1. `bsp_UART.h` not found
- **原因**: scheduler.c（F103模板遗留文件）引用了不存在的头文件
- **修复**: 清空 scheduler.c

### 2. `ADC_SampleTime_55Cycles` undefined
- **原因**: F407 ADC 采样时间常量名不同
- **修复**: 改为 `ADC_SampleTime_56Cycles`

### 3. `BKP_DR1` / `RTC_WaitForLastTask` undefined
- **原因**: F407 RTC API 与 F103 完全不同
- **修复**: 重写 bsp_rtc.c，使用 F407 日历型 RTC API

### 4. `MPU6050_Available` multiply defined
- **原因**: main.c 和 task_sensor.c 中重复定义
- **修复**: main.c 改为 `extern uint8_t MPU6050_Available`

### 5. `GREEN`/`BLACK`/`WHITE` undefined
- **原因**: scheduler.c 引用 LCD 颜色宏
- **修复**: 清空 scheduler.c

### 6. FreeRTOS 调度器启动 HardFault（LSPERR）
- **原因**: 使用 Cortex-M3 版 port.c，不保存 FPU 寄存器 S16-S31
- **修复**: 从 GitHub FreeRTOS-Kernel 下载官方 RVDS/ARM_CM4F 端口替换
- **详情**: 见 `技术困难攻克.md` 2.7 节

---

## OLED 引脚说明

| 信号 | 引脚 | 说明 |
|------|------|------|
| SCL | PB14 | 软件 I2C 时钟线 |
| SDA | PB15 | 软件 I2C 数据线 |

---

## 注意事项

1. **PA0 引脚冲突**: KEY1 和 ADC123_IN0 共用 PA0，当前 KEY1 优先，ADC 功能不可用
2. **scheduler.c**: 该文件已清空，是 F103 模板遗留文件
3. **首次烧录**: 需要通过 UI 设置时间（RTC 首次校时）
4. **FreeRTOS 端口**: 必须使用 RVDS/ARM_CM4F，不能使用 ARM_CM3
