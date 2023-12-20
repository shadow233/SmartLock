/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:24:55
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-19 16:18:07
 * @FilePath: \SmartLock\components\led\include\led.h
 * @Description:
 */
#ifndef __LED_H
#define __LED_H

#include "driver/gpio.h"

#define LED1_PIN GPIO_NUM_12
#define LED2_PIN GPIO_NUM_13

#define LED1(x)                                                        \
    do                                                                 \
    {                                                                  \
        x ? gpio_set_level(LED1_PIN, 1) : gpio_set_level(LED1_PIN, 0); \
    } while (0) /* LED */

/* LED取反定义 */
#define LED1_TOGGLE()                                        \
    do                                                       \
    {                                                        \
        gpio_set_level(LED1_PIN, !gpio_get_level(LED1_PIN)); \
    } while (0) /* Toggle LED */

#define LED2(x)                                                        \
    do                                                                 \
    {                                                                  \
        x ? gpio_set_level(LED2_PIN, 1) : gpio_set_level(LED2_PIN, 0); \
    } while (0) /* LED */

/* LED取反定义 */
#define LED2_TOGGLE()                                        \
    do                                                       \
    {                                                        \
        gpio_set_level(LED2_PIN, !gpio_get_level(LED2_PIN)); \
    } while (0) /* Toggle LED */

void led_init(void);

#endif
