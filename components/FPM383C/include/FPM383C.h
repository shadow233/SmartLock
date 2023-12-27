/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 15:42:09
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-27 17:08:44
 * @FilePath: \SmartLock\components\FPM383C\include\FPM383C.h
 * @Description:
 */

#ifndef __FPM383C_H
#define __FPM383C_H

#include "driver/uart.h"

typedef struct
{
    char *tag;
    char id[16]; // 模块ID
    int password;
    QueueHandle_t uart_queue;
    QueueHandle_t queue;
    TaskHandle_t taskHandle;
    TaskHandle_t rTaskHandle;
} FPM383C_TypeDef, *PFPM383C_TypeDef;

void FPM383C_Init(PFPM383C_TypeDef p);

#endif
