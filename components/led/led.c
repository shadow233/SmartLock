/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-19 15:24:55
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-19 16:19:05
 * @FilePath: \SmartLock\components\led\led.c
 * @Description:
 */
#include <stdio.h>
#include "led.h"

void led_init(void)
{
    gpio_config_t led_pin_config;
    /* LED1 D4 */
    led_pin_config.pin_bit_mask = 1 << LED1_PIN;
    led_pin_config.mode = GPIO_MODE_INPUT_OUTPUT;
    led_pin_config.pull_up_en = GPIO_PULLUP_DISABLE;
    led_pin_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    led_pin_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&led_pin_config);
    /* LED2 D5 */
    led_pin_config.pin_bit_mask = 1 << LED2_PIN;
    led_pin_config.mode = GPIO_MODE_INPUT_OUTPUT;
    led_pin_config.pull_up_en = GPIO_PULLUP_DISABLE;
    led_pin_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    led_pin_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&led_pin_config);

    LED1(0);
    LED2(0);
}
