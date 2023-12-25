#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MG90S.h"
#include "FPM383C.h"
#include "esp_log.h"

typedef struct
{
    char *tag;
    PFPM383C_TypeDef pFPM383C;
} RunParam, *PRunParam;

#endif