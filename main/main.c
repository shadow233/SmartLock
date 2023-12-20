/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:22:02
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-20 10:57:59
 * @FilePath: \SmartLock\main\main.c
 * @Description:
 */
#include <stdio.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    led_init();
    while (1)
    {
        printf("hello world\r\n");
        LED1_TOGGLE();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
