/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:22:02
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-21 15:34:18
 * @FilePath: \SmartLock\main\main.c
 * @Description:
 */
#include <stdio.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MG90S.h"

#define VERSION "0.1.1"

static TaskHandle_t LED_Task_Handle = NULL;   /* LED 任务句柄 */
static TaskHandle_t MG90S_Task_Handle = NULL; /* MG90S 任务句柄 */

static void LED_Task(void *pvParameters);   /* LED_Task 任务实现 */
static void MG90S_Task(void *pvParameters); /* MG90S_Task 任务实现 */

void app_main(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为 pdPASS */
    led_init();
    MG90S_Init();

    printf("\r\n================================\r\n");
    printf("\r\n-----SMARTLOCK START %s-----\r\n", VERSION);
    printf("\r\n================================\r\n");

    /* 创建 LED_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)LED_Task,          /* 任务入口函数 */
                          (const char *)"LED_Task",          /* 任务名字 */
                          (uint16_t)512,                     /* 任务栈大小 */
                          (void *)NULL,                      /* 任务入口函数参数 */
                          (UBaseType_t)2,                    /* 任务的优先级 */
                          (TaskHandle_t *)&LED_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        printf("创建 LED_Task 任务成功!\r\n");
    else
        return;
    /* 创建 MG90S_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)MG90S_Task,          /* 任务入口函数 */
                          (const char *)"MG90S_Task",          /* 任务名字 */
                          (uint16_t)512,                       /* 任务栈大小 */
                          (void *)NULL,                        /* 任务入口函数参数 */
                          (UBaseType_t)3,                      /* 任务的优先级 */
                          (TaskHandle_t *)&MG90S_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        printf("创建 MG90S_Task 任务成功!\r\n");
    else
        return;

    /* 启动任务调度 */
    // vTaskStartScheduler();
}

void LED_Task(void *pvParameters)
{
    while (1)
    {
        printf("LED TASK\r\n");
        LED1_TOGGLE();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void MG90S_Task(void *pvParameters)
{
    while (1)
    {
        printf("MG90S TASK\r\n");
        MG90S_360_Forward();
    }
}
