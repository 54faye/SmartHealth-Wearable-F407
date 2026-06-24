#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f4xx.h"
#include <stdint.h>

extern uint32_t SystemCoreClock;

/* Core settings */
#define configUSE_PREEMPTION                        1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION     1
#define configCPU_CLOCK_HZ                          SystemCoreClock
#define configTICK_RATE_HZ                          1000
#define configMAX_PRIORITIES                        8
#define configMINIMAL_STACK_SIZE                    128
#define configMAX_TASK_NAME_LEN                     8
#define configUSE_16_BIT_TICKS                      0
#define configIDLE_SHOULD_YIELD                     1
#define configUSE_TASK_NOTIFICATIONS                1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES       1

/* Memory */
#define configSUPPORT_STATIC_ALLOCATION             1
#define configSUPPORT_DYNAMIC_ALLOCATION            1
#define configTOTAL_HEAP_SIZE                       ((size_t)(32768))
#define configAPPLICATION_ALLOCATED_HEAP            0

/* Hooks */
#define configUSE_IDLE_HOOK                         1
#define configUSE_TICK_HOOK                         0
#define configCHECK_FOR_STACK_OVERFLOW              2
#define configUSE_MALLOC_FAILED_HOOK                1

/* Timers */
#define configUSE_TIMERS                            1
#define configTIMER_TASK_PRIORITY                   (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                    5
#define configTIMER_TASK_STACK_DEPTH                80

/* Mutex/Queue */
#define configUSE_MUTEXES                           1
#define configUSE_RECURSIVE_MUTEXES                 0
#define configUSE_COUNTING_SEMAPHORES               0
#define configQUEUE_REGISTRY_SIZE                   0
#define configUSE_QUEUE_SETS                        0
#define configUSE_TIME_SLICING                      1
#define configUSE_NEWLIB_REENTRANT                  0
#define configENABLE_BACKWARD_COMPATIBILITY         0

/* Optional functions */
#define INCLUDE_vTaskPrioritySet                    1
#define INCLUDE_uxTaskPriorityGet                   1
#define INCLUDE_vTaskDelete                         1
#define INCLUDE_vTaskSuspend                        1
#define INCLUDE_vTaskDelayUntil                     1
#define INCLUDE_vTaskDelay                          1
#define INCLUDE_xTaskGetSchedulerState              1
#define INCLUDE_xTaskGetCurrentTaskHandle           1
#define INCLUDE_uxTaskGetStackHighWaterMark         1
#define INCLUDE_eTaskGetState                       1
#define INCLUDE_xTimerPendFunctionCall              1
#define INCLUDE_xTaskAbortDelay                     1
#define INCLUDE_xTaskGetHandle                      1
#define INCLUDE_xTaskResumeFromISR                  1

/* Interrupt priority - STM32F407 has 4 priority bits */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS 4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
#define configKERNEL_INTERRUPT_PRIORITY     (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Map FreeRTOS handlers to CMSIS names */
#define xPortPendSVHandler      PendSV_Handler
#define vPortSVCHandler         SVC_Handler

/* Assert */
#define configASSERT(x) if((x) == 0) { taskDISABLE_INTERRUPTS(); for(;;); }

#endif /* FREERTOS_CONFIG_H */
