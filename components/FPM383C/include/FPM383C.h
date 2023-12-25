/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 15:42:09
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-25 10:10:31
 * @FilePath: \SmartLock\components\FPM383C\include\FPM383C.h
 * @Description:
 */

#ifndef __FPM383C_H
#define __FPM383C_H

#include "driver/uart.h"

typedef struct
{
    char *tag;
    char *rTag;
    QueueHandle_t queue;
    TaskHandle_t taskHandle;
    TaskHandle_t rTaskHandle;
} FPM383C_TypeDef, *PFPM383C_TypeDef;

void FPM383C_Init(PFPM383C_TypeDef p);
void FPM383C_Task(void *pvParameters);
void FPM383C_Recv_Task(void *pvParameters);

#endif
