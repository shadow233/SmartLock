/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 09:29:04
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-25 11:52:33
 * @FilePath: \SmartLock\components\MG90S\include\MG90S.h
 * @Description:
 */
#ifndef __MG90S_H
#define __MG90S_H

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define DEGREE_0_EVENT (0x01 << 0)   // 设置事件掩码的位 0
#define DEGREE_90_EVENT (0x01 << 1)  // 设置事件掩码的位 1
#define DEGREE_180_EVENT (0x01 << 2) // 设置事件掩码的位 2

typedef struct
{
    char *tag;
    TaskHandle_t taskHandle;
    EventGroupHandle_t eventHandle;
} MG90S_TypeDef, *PMG90S_TypeDef;

void MG90S_Init(void);
void MG90S_Task(void *pvParameters);

#endif
