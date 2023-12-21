/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 15:42:09
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-21 16:11:38
 * @FilePath: \SmartLock\components\FPM383C\FPM383C.c
 * @Description:
 */
#include <stdio.h>
#include "FPM383C.h"
#include "driver/gpio.h"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_0)
#define RXD_PIN (GPIO_NUM_1)

void FPM383C_Init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
