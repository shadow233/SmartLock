/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:22:02
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-27 14:20:49
 * @FilePath: \SmartLock\main\main.c
 * @Description:
 */

#include "main.h"

#define VERSION "V0.1.8"

PRunParam_t pRunParam = NULL;

static TaskHandle_t MAIN_Task_Handle = NULL;

static void MAIN_Task(void *pvParameters);

void app_main(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为 pdPASS */

    /* pRunParam Init */
    pRunParam = malloc(sizeof(RunParam_t));
    pRunParam->tag = "app_main";
    esp_log_level_set(pRunParam->tag, ESP_LOG_INFO);
    pRunParam->pMG90S = malloc(sizeof(MG90S_TypeDef));
    pRunParam->pFPM383C = malloc(sizeof(FPM383C_TypeDef));
    bzero(pRunParam->pMG90S, sizeof(MG90S_TypeDef));
    bzero(pRunParam->pFPM383C, sizeof(FPM383C_TypeDef));
    pRunParam->pFPM383C->queue = xQueueCreate(20, 4);
    if (pRunParam->pFPM383C->queue != NULL)
        ESP_LOGI(pRunParam->tag, "Create FPM383C_Queue Success!");
    pRunParam->pMG90S->eventHandle = xEventGroupCreate();
    if (pRunParam->pMG90S->eventHandle != NULL)
        ESP_LOGI(pRunParam->tag, "Create MG90S_Event Success!");

    /* components Init */
    led_init();
    MG90S_Init();
    FPM383C_Init(pRunParam->pFPM383C);

    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(pRunParam->tag, "SMARTLOCK START %s", VERSION);

    /* Create Task */
    xReturn = xTaskCreate((TaskFunction_t)MAIN_Task,
                          (const char *)"MAIN_Task",
                          (uint16_t)2048,
                          (void *)(pRunParam),
                          (UBaseType_t)2,
                          (TaskHandle_t *)&(MAIN_Task_Handle));
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "Create MAIN_Task Success!");
    else
        return;
    xReturn = xTaskCreate((TaskFunction_t)MG90S_Task,
                          (const char *)"MG90S_Task",
                          (uint16_t)2048,
                          (void *)(pRunParam->pMG90S),
                          (UBaseType_t)3,
                          (TaskHandle_t *)&(pRunParam->pMG90S->taskHandle));
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "Create MG90S_Task Success!");
    else
        return;
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Task,
                          (const char *)"FPM383C_Task",
                          (uint16_t)4096,
                          (void *)(pRunParam->pFPM383C),
                          (UBaseType_t)4,
                          (TaskHandle_t *)&(pRunParam->pFPM383C->taskHandle));
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "Create FPM383C_Task Success!");
    else
        return;
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Recv_Task,
                          (const char *)"FPM383C_Recv_Task",
                          (uint16_t)4096,
                          (void *)(pRunParam->pFPM383C),
                          (UBaseType_t)5,
                          (TaskHandle_t *)&(pRunParam->pFPM383C->rTaskHandle));
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "Create FPM383C_Recv_Task Success!");
    else
        return;

    /* 启动任务调度 */
    // vTaskStartScheduler();
}

void MAIN_Task(void *pvParameters)
{
    // PRunParam_t p = (PRunParam_t)pvParameters;
    // const char *tag = "main";
    // esp_log_level_set(tag, ESP_LOG_INFO);
    while (1)
    {
        // xEventGroupSetBits(p->pMG90S->eventHandle, DEGREE_0_EVENT);
        // vTaskDelay(pdMS_TO_TICKS(1000));
        // xEventGroupSetBits(p->pMG90S->eventHandle, DEGREE_90_EVENT);
        // vTaskDelay(pdMS_TO_TICKS(1000));
        // xEventGroupSetBits(p->pMG90S->eventHandle, DEGREE_180_EVENT);
        // ESP_LOGI(tag, "hello world");
        vTaskDelay(pdMS_TO_TICKS(1000));
        // vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
