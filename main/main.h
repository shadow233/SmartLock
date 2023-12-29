/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-25 09:13:17
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-26 14:36:19
 * @FilePath: \SmartLock\main\main.h
 * @Description:
 */
#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "MG90S.h"
#include "FPM383C.h"
#include "esp_log.h"
#include "string.h"
#include "wifi_mqtt.h"

typedef struct
{
    char *tag;
    PMG90S_TypeDef pMG90S;
    PFPM383C_TypeDef pFPM383C;
} RunParam_t, *PRunParam_t;

#endif