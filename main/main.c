/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:22:02
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-22 17:57:27
 * @FilePath: \SmartLock\main\main.c
 * @Description:
 */
#include <stdio.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MG90S.h"
#include "FPM383C.h"
#include "esp_log.h"

#define VERSION "V0.1.3"

static TaskHandle_t MG90S_Task_Handle = NULL;   /* MG90S 任务句柄 */
static TaskHandle_t FPM383C_Task_Handle = NULL; /* FPM383C 任务句柄 */
static TaskHandle_t FPM383C_Recv_Task_Handle = NULL; /* FPM383C 任务句柄 */

static void MG90S_Task(void *pvParameters);   /* MG90S_Task 任务实现 */
static void FPM383C_Task(void *pvParameters); /* FPM383C_Task 任务实现 */

void app_main(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为 pdPASS */
    static const char *MAIN_TAG = "app_main";
    esp_log_level_set(MAIN_TAG, ESP_LOG_INFO);
    led_init();
    MG90S_Init();
    FPM383C_Init();

    ESP_LOGI(MAIN_TAG, "SMARTLOCK START %s", VERSION);

    /* 创建 MG90S_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)MG90S_Task, (const char *)"MG90S_Task", (uint16_t)2048, (void *)NULL, (UBaseType_t)2, (TaskHandle_t *)&MG90S_Task_Handle);
    if (pdPASS == xReturn)
        ESP_LOGI(MAIN_TAG, "创建 MG90S_Task 任务成功!\r\n");
    else
        return;
    /* 创建 FPM383C_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Task, (const char *)"FPM383C_Task", (uint16_t)2048, (void *)NULL, (UBaseType_t)3, (TaskHandle_t *)&FPM383C_Task_Handle);
    if (pdPASS == xReturn)
        ESP_LOGI(MAIN_TAG, "创建 FPM383C_Task 任务成功!\r\n");
    else
        return;
    /* 创建 FPM383C_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Recv_Task, (const char *)"FPM383C_Recv_Task", (uint16_t)3096, (void *)NULL, (UBaseType_t)4, (TaskHandle_t *)&FPM383C_Recv_Task_Handle);
    if (pdPASS == xReturn)
        ESP_LOGI(MAIN_TAG, "创建 FPM383C_Task 任务成功!\r\n");
    else
        return;

    /* 启动任务调度 */
    // vTaskStartScheduler();
}

void MG90S_Task(void *pvParameters)
{
    static const char *MG90S_TASK_TAG = "MG90S_Task";
    esp_log_level_set(MG90S_TASK_TAG, ESP_LOG_INFO);
    while (1)
    {
        // ESP_LOGI(MG90S_TASK_TAG, "MG90S TASK\r\n");
        // MG90S_360_Forward();
        LED1_TOGGLE();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void FPM383C_Task(void *pvParameters)
{
    static const char *FPM383C_TASK_TAG = "FPM383C_Task";
    esp_log_level_set(FPM383C_TASK_TAG, ESP_LOG_INFO);
    uint8_t buf[] = "hello world\r\n";
    while (1)
    {
        // ESP_LOGI(FPM383C_TASK_TAG, "FPM383C TASK\r\n");
        // FPM383C_Send_Bytes(buf, sizeof(buf));
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


