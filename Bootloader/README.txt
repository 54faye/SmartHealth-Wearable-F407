========================================
  STM32F407VE OTA Bootloader
========================================

1. Keil工程创建步骤:
   - 新建Keil工程，选择STM32F407VE
   - 添加以下源文件:

   [Bootloader/User/]
     main.c
     stm32f4xx_it.c

   [Bootloader/Drivers/BSP/] (Bootloader专用)
     bsp_delay.c
     bsp_usart.c

   [../Drivers/BSP/] (共享，从App工程引用)
     bsp_spi1.c
     bsp_w25q128.c
     bsp_i2c_hw.c
     bsp_at24c02.c

   [../Drivers/CMSIS/]
     system_stm32f4xx.c
     startup_stm32f40xx.s (Keil版)

   [../Drivers/STM32F4xx_StdPeriph_Driver/src/]
     stm32f4xx_flash.c
     stm32f4xx_gpio.c
     stm32f4xx_rcc.c
     stm32f4xx_spi.c
     stm32f4xx_i2c.c
     stm32f4xx_usart.c
     stm32f4xx_pwr.c
     stm32f4xx_syscfg.c
     stm32f4xx_crc.c
     misc.c

2. Include路径:
     Bootloader/User
     Bootloader/Drivers/BSP
     Drivers/BSP           (共享BSP)
     Drivers/CMSIS
     Drivers/STM32F4xx_StdPeriph_Driver/inc

3. 预编译宏:
     USE_STDPERIPH_DRIVER, STM32F40_41xxx

4. 散列文件:
     使用 Bootloader/Output/Bootloader.sct

5. Flash下载:
     起始地址: 0x08000000
     大小: 0x10000 (64KB)

========================================
  分区表
========================================

  0x08000000 +----------------+ 64KB
             |  Bootloader    |
  0x08010000 +----------------+ 192KB
             |  App_A         |
  0x08040000 +----------------+ 256KB
             |  App_B         |
  0x08080000 +----------------+

========================================
  OTA流程
========================================

  1. App通过ESP8266下载固件到W25Q128
  2. App设置BootFlags (ota_pending=1, ota_target)
  3. App重启
  4. Bootloader检测OTA标志
  5. 从W25Q128校验并写入目标分区
  6. 切换active_partition
  7. 跳转到新分区运行
