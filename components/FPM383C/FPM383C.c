/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 15:42:09
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-22 17:42:21
 * @FilePath: \SmartLock\components\FPM383C\FPM383C.c
 * @Description:
 */
#include <stdio.h>
#include <string.h>
#include "FPM383C.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TXD_PIN (GPIO_NUM_0)
#define RXD_PIN (GPIO_NUM_1)

#define EX_UART_NUM UART_NUM_1 // 定义串口编号宏定义

#define BUF_SIZE (1024)           // buf大小
#define RD_BUF_SIZE (BUF_SIZE)    // 读取字符大小
static QueueHandle_t uart0_queue; // 串口队列

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
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);
    uart_set_pin(EX_UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void FPM383C_Send_Bytes(uint8_t *buf, uint16_t len)
{
    uart_write_bytes(EX_UART_NUM, buf, len);
}

void FPM383C_Recv_Task(void *pvParameters)
{
    static const char *FPM383C_Recv_TASK_TAG = "FPM383C_Recv_Task";
    esp_log_level_set(FPM383C_Recv_TASK_TAG, ESP_LOG_INFO);
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE); // 动态申请内存
    for (;;)
    {
        // Waiting for UART event.                   等待串口事件队列
        if (xQueueReceive(uart0_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            bzero(dtmp, RD_BUF_SIZE); // 清空动态申请的队列
            ESP_LOGI(FPM383C_Recv_TASK_TAG, "uart[%d] event:", EX_UART_NUM);
            switch (event.type)
            {
            case UART_DATA: // UART接收数据的事件
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY); // 获取数据
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "[DATA EVT]:");
                FPM383C_Send_Bytes(dtmp, event.size);
                // uart_write_bytes(EX_UART_NUM, (const char *)dtmp, event.size); // 发送数据
                break;
            case UART_FIFO_OVF: // 检测到硬件 FIFO 溢出事件
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "hw fifo overflow");
                // 如果fifo溢出发生，你应该考虑为你的应用程序添加流量控制。
                // ISR已经重置了rx FIFO，例如，我们直接刷新rx缓冲区来读取更多的数据。
                uart_flush_input(EX_UART_NUM); // 清除输入缓冲区，丢弃所有环缓冲区中的数据
                xQueueReset(uart0_queue);      // 重置一个队列到它原来的空状态。返回值是现在过时，并且总是设置为pdPASS。
                break;
            case UART_BUFFER_FULL: // UART RX 缓冲器满事件
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "ring buffer full");
                // 如果缓冲区满了，你应该考虑增加你的缓冲区大小
                // 举个例子，我们这里直接刷新 rx 缓冲区，以便读取更多数据。uart_flush_input(EX_UART_NUM);
                xQueueReset(uart0_queue); // 重置一个队列到它原来的空状态
                break;
            case UART_BREAK: // UART 中断事件
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "uart rx break");
                break;
            case UART_PARITY_ERR: // UART奇偶校验错误事件
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "uart parity error");
                break;
            case UART_FRAME_ERR: // UART 帧错误事件
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "uart frame error");
                break;
            default: // 其他
                ESP_LOGI(FPM383C_Recv_TASK_TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);        // 释放内存
    dtmp = NULL;       // 清除内存指针
    vTaskDelete(NULL); // 删除任务
}
