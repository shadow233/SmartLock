/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:22:02
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-25 10:15:44
 * @FilePath: \SmartLock\main\main.c
 * @Description:
 */

#include "main.h"

#define VERSION "V0.1.4"

PRunParam pRunParam = NULL;

static TaskHandle_t MG90S_Task_Handle = NULL; /* MG90S 任务句柄 */

static void MG90S_Task(void *pvParameters); /* MG90S_Task 任务实现 */

void app_main(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为 pdPASS */

    pRunParam = malloc(sizeof(RunParam));
    pRunParam->tag = "app_main";
    pRunParam->pFPM383C = malloc(sizeof(FPM383C_TypeDef));

    esp_log_level_set(pRunParam->tag, ESP_LOG_INFO);
    led_init();
    MG90S_Init();
    FPM383C_Init(pRunParam->pFPM383C);

    ESP_LOGI(pRunParam->tag, "SMARTLOCK START %s", VERSION);

    /* 创建 MG90S_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)MG90S_Task, (const char *)"MG90S_Task", (uint16_t)2048, (void *)NULL, (UBaseType_t)2, (TaskHandle_t *)&MG90S_Task_Handle);
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "创建 MG90S_Task 任务成功!");
    else
        return;
    /* 创建 FPM383C_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Task, (const char *)"FPM383C_Task", (uint16_t)2048, (void *)NULL, (UBaseType_t)3, (TaskHandle_t *)&(pRunParam->pFPM383C->FPM383C_Task_Handle));
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "创建 FPM383C_Task 任务成功!");
    else
        return;
    /* 创建 FPM383C_Recv_Task 任务 */
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Recv_Task, (const char *)"FPM383C_Recv_Task", (uint16_t)2048, (void *)(pRunParam->pFPM383C), (UBaseType_t)4, (TaskHandle_t *)&(pRunParam->pFPM383C->FPM383C_Recv_Task_Handle));
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "创建 FPM383C_Recv_Task 任务成功!");
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
        // ESP_LOGI(MG90S_TASK_TAG, "MG90S TASK");
        // MG90S_360_Forward();
        LED1_TOGGLE();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
