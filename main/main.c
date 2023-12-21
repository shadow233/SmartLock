/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:22:02
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-21 11:16:07
 * @FilePath: \SmartLock\main\main.c
 * @Description:
 */
#include <stdio.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MG90S.h"

#define VERSION "0.1.0"

void app_main(void)
{
    printf("\r\n================================\r\n");
    printf("\r\n-----SMARTLOCK START %s-----\r\n", VERSION);
    printf("\r\n================================\r\n");

    led_init();
    MG90S_Init();
    while (1)
    {
        printf("hello world\r\n");
        LED1_TOGGLE();
        MG90S_360_Forward();
        // vTaskDelay(1000 / portTICK_RATE_MS);
        // vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
