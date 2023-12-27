/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-21 15:42:09
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2023-12-27 16:56:11
 * @FilePath: \SmartLock\components\FPM383C\FPM383C.c
 * @Description:
 */
#include <stdio.h>
#include <string.h>
#include "FPM383C.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "led.h"

#define TXD_PIN (GPIO_NUM_0)
#define RXD_PIN (GPIO_NUM_1)

#define EX_UART_NUM UART_NUM_1 // 定义串口编号宏定义

#define BUF_SIZE (256)         // buf大小
#define RD_BUF_SIZE (BUF_SIZE) // 读取字符大小

#define TOUCH_OUT_PIN (GPIO_NUM_3)

typedef enum
{
    MATCH = 0,
    AUTOLOGON,
    DELETE
} FPM383C_Event_Typedef;

typedef enum
{
    FPM383C_OK = 0,
    FPM383C_ERROR,
    FPM383C_TIMEOUT,
    FPM383C_AWAKEN
} FPM383C_State_Typedef;

/* 直接使用 */
/* 心跳 */
uint8_t heartBeat[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x07, 0x86, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0xFA};
/* 获取模块ID */
uint8_t getModuleID[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x07, 0x86, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, 0xFC};
/* 自动注册 */
uint8_t autoLogon[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x0B, 0x82, 0x00, 0x00, 0x00, 0x00, 0x01, 0x18, 0x01, 0x06, 0xFF, 0xFF, 0xE2};
/* 指纹匹配（同步） */
uint8_t match[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x07, 0x86, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0xDC};
/* LED 关闭 */
uint8_t led_close[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x0C, 0x81, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF};
/* LED 开启*/
uint8_t led_red[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x0C, 0x81, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0F, 0x01, 0x02, 0x00, 0x00, 0x00, 0xEC};
uint8_t led_green[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x0C, 0x81, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0F, 0x01, 0x01, 0x00, 0x00, 0x00, 0xED};
uint8_t led_blue[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x0C, 0x81, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0F, 0x01, 0x04, 0x00, 0x00, 0x00, 0xEA};
/* 休眠 */
uint8_t normalSleep[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x08, 0x85, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0C, 0x00, 0xF2};
uint8_t deepSleep[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x08, 0x85, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0C, 0x01, 0xF1};

/* 需要修改 */
/* 指纹特征清除（同步） */
uint8_t delete[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A, 0x00, 0x0A, 0x83, 0x00, 0x00, 0x00, 0x00, 0x01, 0x36, 0x00, 0x00, 0x04, 0xC5};

static void IRAM_ATTR TOUCH_OUT_IRQHandler(void *arg)
{
    QueueHandle_t queue = (QueueHandle_t)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint8_t s_queue[4] = {MATCH, FPM383C_AWAKEN, 0, 0};

    xQueueSendFromISR(queue, s_queue, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

PFPM383C_TypeDef pFPM383C = NULL;

void FPM383C_Init(PFPM383C_TypeDef p)
{
    BaseType_t xReturn = pdPASS;
    pFPM383C = malloc(sizeof(FPM383C_TypeDef));
    p = pFPM383C;
    bzero(p, sizeof(FPM383C_TypeDef));
    p->tag = "FPM383C_Task";
    esp_log_level_set(p->tag, ESP_LOG_INFO);

    p->queue = xQueueCreate(20, 4);
    if (p->queue != NULL)
        ESP_LOGI(p->tag, "Create FPM383C_Queue Success!");

    const uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &(p->uart_queue), 0);
    uart_param_config(EX_UART_NUM, &uart_config);
    uart_set_pin(EX_UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    const gpio_config_t io_conf = {
        .pin_bit_mask = 1 << TOUCH_OUT_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&io_conf);
    gpio_set_intr_type(TOUCH_OUT_PIN, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(TOUCH_OUT_PIN, TOUCH_OUT_IRQHandler, (void *)p->queue);
    gpio_isr_handler_remove(TOUCH_OUT_PIN);

    /* Create Task */
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Task,
                          (const char *)"FPM383C_Task",
                          (uint16_t)4096,
                          (void *)(p),
                          (UBaseType_t)4,
                          (TaskHandle_t *)&(p->taskHandle));
    if (pdPASS == xReturn)
        ESP_LOGI(p->tag, "Create FPM383C_Task Success!");
    else
        return;
    xReturn = xTaskCreate((TaskFunction_t)FPM383C_Recv_Task,
                          (const char *)"FPM383C_Recv_Task",
                          (uint16_t)4096,
                          (void *)(p),
                          (UBaseType_t)5,
                          (TaskHandle_t *)&(p->rTaskHandle));
    if (pdPASS == xReturn)
        ESP_LOGI(p->tag, "Create FPM383C_Recv_Task Success!");
    else
        return;
}

/**
 * @brief LRC Check
 * 帧头校验和 = ~（帧头+应用层数据长度的校验） + 1
 * 发送校验和 = ~ (校验密码+命令+数据内容) + 1
 * 接收校验和 = ~（校验密码+响应命令+错误码+数据内容） + 1
 * @param {uint8_t} *data
 * @param {uint16_t} length
 * @return {uint8_t}
 */
static uint8_t LRC_Check(uint8_t *data, uint16_t length)
{
    uint16_t lrc = 0;
    for (uint16_t i = 0; i < length; i++)
    {
        lrc += data[i];
    }
    return (uint8_t)((~lrc) + 1);
}

void FPM383C_Task(void *pvParameters)
{
    PFPM383C_TypeDef p = (PFPM383C_TypeDef)pvParameters;

    /* 获取模块ID */
    uart_write_bytes(EX_UART_NUM, getModuleID, sizeof(getModuleID));
    vTaskDelay(pdMS_TO_TICKS(200));

    uart_write_bytes(EX_UART_NUM, led_blue, sizeof(led_blue));
    vTaskDelay(pdMS_TO_TICKS(200));

    uart_write_bytes(EX_UART_NUM, match, sizeof(match));

    BaseType_t xReturn = pdTRUE;
    uint8_t r_queue[4];
    uint16_t id = 0;
    static uint8_t count = 0;
    while (1)
    {
        xReturn = xQueueReceive(p->queue, r_queue, portMAX_DELAY);
        if (xReturn == pdTRUE)
        {
            switch (r_queue[0])
            {
            case MATCH:
                if (r_queue[1] == FPM383C_OK)
                {
                    id = r_queue[2] << 8 | r_queue[3];
                    ESP_LOGI(p->tag, "id:%d", id);
                    if (id == 0 || id == 2 || id == 4)
                    {
                        /* 关锁 */
                    }
                    else if (id == 1 || id == 3)
                    {
                        /* 开锁 */
                    }

                    uart_write_bytes(EX_UART_NUM, led_green, sizeof(led_green));
                    vTaskDelay(pdMS_TO_TICKS(200));
                    uart_write_bytes(EX_UART_NUM, led_blue, sizeof(led_blue));
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
                else if (r_queue[1] == FPM383C_TIMEOUT)
                {
                    count++;
                    if (count == 3)
                    {
                        uart_write_bytes(EX_UART_NUM, led_close, sizeof(led_close));
                        vTaskDelay(pdMS_TO_TICKS(200));
                        gpio_isr_handler_add(TOUCH_OUT_PIN, TOUCH_OUT_IRQHandler, (void *)p->queue);
                        uart_write_bytes(EX_UART_NUM, normalSleep, sizeof(normalSleep));
                        vTaskDelay(pdMS_TO_TICKS(200));
                        count = 0;
                    }
                }
                else if (r_queue[1] == FPM383C_AWAKEN)
                {
                    ESP_LOGI(p->tag, "[AWAKEN]");
                    // remove isr handler for gpio number.
                    gpio_isr_handler_remove(TOUCH_OUT_PIN);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    uart_write_bytes(EX_UART_NUM, led_blue, sizeof(led_blue));
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
                else
                {
                    uart_write_bytes(EX_UART_NUM, led_red, sizeof(led_red));
                    vTaskDelay(pdMS_TO_TICKS(200));
                    uart_write_bytes(EX_UART_NUM, led_blue, sizeof(led_blue));
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
                uart_write_bytes(EX_UART_NUM, match, sizeof(match));
                break;
            case AUTOLOGON:
                break;
            case DELETE:
                break;
            default:
                break;
            }
        }
        else
            ESP_LOGI(p->tag, "[ERROR] %d %s %d", xReturn, __FUNCTION__, __LINE__);
    }
}

static void FPM383C_Recv_IRQHandler(PFPM383C_TypeDef p, uint8_t *data, uint16_t size)
{
    /* 帧头校验 */
    if (data[10] != LRC_Check(data, 10))
    {
        ESP_LOGI(p->tag, "[ERROR] LRC %s %d", __FUNCTION__, __LINE__);
        ESP_LOGI(p->tag, "True Value : %02X", LRC_Check(data, 10));
        return;
    }
    /* 校验和 */
    if (data[size - 1] != LRC_Check(data + 11, size - 12))
    {
        ESP_LOGI(p->tag, "[ERROR] LRC %s %d", __FUNCTION__, __LINE__);
        ESP_LOGI(p->tag, "True Value : %02X", LRC_Check(data + 11, size - 12));
        return;
    }
    /* 校验密码 */
    if ((data[11] << 24 | data[12] << 16 | data[13] << 8 | data[14]) != p->password)
    {
        ESP_LOGI(p->tag, "[ERROR] password %s %d", __FUNCTION__, __LINE__);
        return;
    }

    uint16_t cmd = data[15] << 8 | data[16];
    int error = data[17] << 24 | data[18] << 16 | data[19] << 8 | data[20];
    BaseType_t xReturn = pdPASS;
    uint8_t s_queue[4] = {0};
    /* 错误码 */
    if (error != 0)
    {
        ESP_LOGI(p->tag, "[ERROR] %08X", error);
    }
    switch (cmd)
    {
    case 0x0301: // 获取指纹模块id
        if (error == 0)
        {
            strncpy(p->id, (char *)(data + 21), 16);
            ESP_LOGI(p->tag, "ID:%s", p->id);
        }
        break;
    case 0x0118: // 自动注册
        if (error == 0)
        {
            uint8_t count = data[21];
            uint16_t id = data[22] << 8 | data[23];
            uint8_t progress = data[24];
            if (count == 0xff && progress == 0x64)
            {
                ESP_LOGI(p->tag, "[auto log-on success] id:%d", id);
            }
            else
            {
                ESP_LOGI(p->tag, "[auto log-on progress] id:%d count:%d progress:%d%%", id, count, progress);
            }
        }
        break;
    case 0x0123: // 指纹匹配
        if (error == 0)
        {
            uint16_t result = data[21] << 8 | data[22];
            uint16_t score = data[23] << 8 | data[24];
            uint16_t match_id = data[25] << 8 | data[26];
            if (result != 0)
            {
                ESP_LOGI(p->tag, "[MATCH] id:%d score:%d", match_id, score);
                s_queue[0] = MATCH;
                s_queue[1] = FPM383C_OK;
                s_queue[2] = match_id >> 8;
                s_queue[3] = match_id;
                xReturn = xQueueSend(p->queue, s_queue, 0);
                if (xReturn != pdPASS)
                    ESP_LOGI(p->tag, "[ERROR] %d %s %d", xReturn, __FUNCTION__, __LINE__);
            }
            else
            {
                ESP_LOGI(p->tag, "[MATCH] not found");
                s_queue[0] = MATCH;
                s_queue[1] = FPM383C_ERROR;
                s_queue[2] = 0;
                s_queue[3] = 0;
                xReturn = xQueueSend(p->queue, s_queue, 0);
                if (xReturn != pdPASS)
                    ESP_LOGI(p->tag, "[ERROR] %d %s %d", xReturn, __FUNCTION__, __LINE__);
            }
        }
        else if (error == 0x8 || error == 0x9)
        {
            s_queue[0] = MATCH;
            s_queue[1] = FPM383C_TIMEOUT;
            s_queue[2] = 0;
            s_queue[3] = 0;
            xReturn = xQueueSend(p->queue, s_queue, 0);
            if (xReturn != pdPASS)
                ESP_LOGI(p->tag, "[ERROR] %d %s %d", xReturn, __FUNCTION__, __LINE__);
        }

        break;
    case 0x0136: // 删除指纹
        if (error == 0)
        {
            ESP_LOGI(p->tag, "[DELETE] success");
        }
        break;
    case 0x020F: // LED
        if (error == 0)
        {
            ESP_LOGI(p->tag, "[LED] change");
        }
        break;
    case 0x020C: // LED
        if (error == 0)
        {
            if (data[17] == 0)
            {
                ESP_LOGI(p->tag, "[Sleep] Enter Normal Sleep");
            }
            else
            {
                ESP_LOGI(p->tag, "[Sleep] Enter Deep Sleep");
            }
        }
        break;
    default:
        break;
    }
}

void FPM383C_Recv_Task(void *pvParameters)
{
    PFPM383C_TypeDef p = (PFPM383C_TypeDef)pvParameters;
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE); // 动态申请内存
    for (;;)
    {
        // Waiting for UART event.                   等待串口事件队列
        if (xQueueReceive(p->uart_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            bzero(dtmp, RD_BUF_SIZE); // 清空动态申请的队列
            switch (event.type)
            {
            case UART_DATA:                                                    // UART接收数据的事件
                uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY); // 获取数据
                ESP_LOGI(p->tag, "rx: %d", event.size);
                for (int i = 0; i < event.size; i++)
                    printf("%02X ", dtmp[i]);
                printf("\r\n");
                FPM383C_Recv_IRQHandler(p, dtmp, event.size);
                break;
            case UART_FIFO_OVF: // 检测到硬件 FIFO 溢出事件
                ESP_LOGI(p->tag, "hw fifo overflow");
                // 如果fifo溢出发生，你应该考虑为你的应用程序添加流量控制。
                // ISR已经重置了rx FIFO，例如，我们直接刷新rx缓冲区来读取更多的数据。
                uart_flush_input(EX_UART_NUM); // 清除输入缓冲区，丢弃所有环缓冲区中的数据
                xQueueReset(p->uart_queue);    // 重置一个队列到它原来的空状态。返回值是现在过时，并且总是设置为pdPASS。
                break;
            case UART_BUFFER_FULL: // UART RX 缓冲器满事件
                ESP_LOGI(p->tag, "ring buffer full");
                // 如果缓冲区满了，你应该考虑增加你的缓冲区大小
                // 举个例子，我们这里直接刷新 rx 缓冲区，以便读取更多数据。uart_flush_input(EX_UART_NUM);
                xQueueReset(p->uart_queue); // 重置一个队列到它原来的空状态
                break;
            case UART_BREAK: // UART 中断事件
                ESP_LOGI(p->tag, "uart rx break");
                break;
            case UART_PARITY_ERR: // UART奇偶校验错误事件
                ESP_LOGI(p->tag, "uart parity error");
                break;
            case UART_FRAME_ERR: // UART 帧错误事件
                ESP_LOGI(p->tag, "uart frame error");
                break;
            default: // 其他
                ESP_LOGI(p->tag, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);        // 释放内存
    dtmp = NULL;       // 清除内存指针
    vTaskDelete(NULL); // 删除任务
}
