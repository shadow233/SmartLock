/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:22:02
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2024-01-02 17:44:14
 * @FilePath: \SmartLock\main\main.c
 * @Description:
 */

#include "main.h"

#define VERSION "V0.2.5"

PRunParam_t pRunParam = NULL;

static TaskHandle_t MAIN_Task_Handle = NULL;

static void MAIN_Task(void *pvParameters);

void app_main(void)
{
    BaseType_t xReturn = pdPASS;

    /* pRunParam Init */
    pRunParam = malloc(sizeof(RunParam_t));
    pRunParam->tag = "app_main";
    esp_log_level_set(pRunParam->tag, ESP_LOG_INFO);

    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI(pRunParam->tag, "SMARTLOCK START %s", VERSION);
    ESP_LOGI(pRunParam->tag, "Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());

    /* components Init */
    mqtt_Init();
    led_init();
    MG90S_Init(pRunParam->pMG90S);
    FPM383C_Init(pRunParam->pFPM383C);

    /* Create Task */
    xReturn = xTaskCreate((TaskFunction_t)MAIN_Task,
                          (const char *)"MAIN_Task",
                          (uint16_t)3072,
                          (void *)(pRunParam),
                          (UBaseType_t)2,
                          (TaskHandle_t *)&(MAIN_Task_Handle));
    if (pdPASS == xReturn)
        ESP_LOGI(pRunParam->tag, "Create MAIN_Task Success!");
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
    // uint8_t heartBeat[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x07, 0x86, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0xFA};

    // while (1)
    // {
    //     // xEventGroupSetBits(p->pMG90S->eventHandle, DEGREE_0_EVENT);
    //     // vTaskDelay(pdMS_TO_TICKS(1000));
    //     // xEventGroupSetBits(p->pMG90S->eventHandle, DEGREE_90_EVENT);
    //     // vTaskDelay(pdMS_TO_TICKS(1000));
    //     // xEventGroupSetBits(p->pMG90S->eventHandle, DEGREE_180_EVENT);
    //     // ESP_LOGI(tag, "hello world");
    //     uart_write_bytes(UART_NUM_1, heartBeat, sizeof(heartBeat));

    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     // vTaskDelay(1000 / portTICK_RATE_MS);
    // }
    uint8_t CPU_RunInfo[400]; // 保存任务运行时间信息

    while (1)
    {
        memset(CPU_RunInfo, 0, 400); // 信息缓冲区清零

        vTaskList((char *)&CPU_RunInfo); // 获取任务运行时间信息

        printf("---------------------------------------------\r\n");
        printf("任务名       任务状态  优先级  剩余栈  任务序号\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n");

        memset(CPU_RunInfo, 0, 400); // 信息缓冲区清零

        vTaskGetRunTimeStats((char *)&CPU_RunInfo);

        printf("任务名\t\t运行计数\t使用率\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
