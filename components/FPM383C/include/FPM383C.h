/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 15:42:09
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-21 15:52:54
 * @FilePath: \SmartLock\components\FPM383C\include\FPM383C.h
 * @Description:
 */

#ifndef __FPM383C_H
#define __FPM383C_H

#include "driver/uart.h"

// typedef struct
// {
//     char *tag;
//     QueueHandle_t queue;

// } FPM383C_TypeDef, *PFPM383C_TypeDef;

void FPM383C_Init(void);
void FPM383C_Send_Bytes(uint8_t *buf, uint16_t len);
void FPM383C_Recv_Task(void *pvParameters);

#endif
