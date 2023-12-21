/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 09:29:04
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-21 11:11:28
 * @FilePath: \SmartLock\components\MG90S\include\MG90S.h
 * @Description:
 */
#ifndef __MG90S_H
#define __MG90S_H

#include "driver/ledc.h"

#define MG90S_360_EN
// #define MG90S_180_EN

void MG90S_Init(void);
#ifdef MG90S_360_EN
void MG90S_360_Forward(void);
#endif
#ifdef MG90S_180_EN
void MG90S_180_Angle(void);
#endif

#endif
