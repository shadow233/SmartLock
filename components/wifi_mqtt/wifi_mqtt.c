/*
 * @Author: shadow MrHload163@163.com
 * @Date: 2023-12-28 17:47:42
 * @LastEditors: shadow MrHload163@163.com
 * @LastEditTime: 2024-01-02 14:52:51
 * @FilePath: \SmartLock\components\wifi_mqtt\wifi_mqtt.c
 * @Description:
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "wifi_mqtt.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "wifi_mqtt_def.h"
#include "esp_pm.h"

/* wifi_mqtt_def.h 中的信息 */
// #define ESP_WIFI_SSID "xx"                 // wifi名
// #define ESP_WIFI_PASS "xxxxxx"             // wifi密码
// #define BEMFA_URI "mqtt://bemfa.com:9501/" // 巴法uri
// #define ID_MQTT "xxxxxxxxxxxxxx"           // 私钥
// #define TOPIC "xxx00x"                     // 主题
// #define DATA_TO_LOCK "xxx"                 // on
// #define DATA_TO_UNLOCK "xxx"               // off

#define ESP_MAXIMUM_RETRY 5
#define DEFAULT_LISTEN_INTERVAL 10
#define DEFAULT_BEACON_TIMEOUT 6

static EventGroupHandle_t s_wifi_event_group;

static const char *TAG = "MQTT";

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static int s_retry_num = 0;

extern void MG90S_Unlock(void);
extern void MG90S_Lock(void);

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        // ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, TOPIC, 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        // printf("%s\r\n", event->topic);

        if (strstr(event->topic, TOPIC) != NULL && strstr(event->data, DATA_TO_UNLOCK) != NULL)
        {
            printf("MG90S_Unlock\r\n");
            MG90S_Unlock();
        }
        else if (strstr(event->topic, TOPIC) != NULL && strstr(event->data, DATA_TO_LOCK) != NULL)
        {
            printf("MG90S_Lock\r\n");
            MG90S_Lock();
        }

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BEMFA_URI,
        .credentials.client_id = ID_MQTT,
    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.broker.address.uri, "FROM_STDIN") == 0)
    {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128)
        {
            int c = fgetc(stdin);
            if (c == '\n')
            {
                line[count] = '\0';
                break;
            }
            else if (c > 0 && c < 127)
            {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.broker.address.uri = line;
        printf("Broker url: %s\n", line);
    }
    else
    {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

// 事件回调
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    // 如果是Wi-Fi事件，并且事件ID是Wi-Fi事件STA_START
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        // 如果是Wi-Fi事件，并且事件ID是Wi-Fi事件STA_DISCONNECTED
        /* 如果重试次数小于最大重试次数 */
        if (s_retry_num < ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        // 如果是IP事件，并且事件ID是IP事件STA_GOT_IP

        // 获取事件结果
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        // 将重试次数重置为 0；
        s_retry_num = 0;
        // 通过调用 xEventGroupSetBits 函数，将 WIFI_CONNECTED_BIT 设置到事件组中，表示成功连接到 AP
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_initialize(void)
{
    // 创建一个事件组，用于管理Wi-Fi连接事件。
    s_wifi_event_group = xEventGroupCreate();

    // 初始化 TCP/IP 协议栈。
    ESP_ERROR_CHECK(esp_netif_init());

    // 创建默认事件循环。
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 创建默认的Wi-Fi网络接口。
    esp_netif_create_default_wifi_sta();
    // 设置 Wi-Fi 初始化配置为默认配置
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册事件处理器，以处理 Wi-Fi 和 IP 相关事件
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
}

static void wifi_station_initialize(void)
{
    // 配置WiFi的配置信息
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .listen_interval = DEFAULT_LISTEN_INTERVAL,
            // 启用WPA2模式，常用的WiFi连接方式
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    // WiFi工作模式设置为STA
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // 设置WiFi工作模式
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    // 启动WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_wifi_set_inactive_time(WIFI_IF_STA, DEFAULT_BEACON_TIMEOUT));

    ESP_LOGI(TAG, "esp_wifi_set_ps().");
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    ESP_LOGI(TAG, "wifi_station_initialize finished.");

    /* 等待连接建立（WIFI_CONNECTED_BIT）或连接失败的次数达到最大值（WIFI_FAIL_BIT）。
     * 这些位通过 event_handler() 设置（详见上面）*/
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() 返回调用前的 bits，因此我们可以测试实际发生了什么事件。 */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

void mqtt_Init(void)
{
    ESP_LOGI(TAG, "MQTT Startup..");

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

/* 由于esp32c3不带REF_TICK，且该开发板没有外部低速时钟源，所以XTAL也无法使用。自动light-sleep模式暂时无法满足条件 */
// #if CONFIG_PM_ENABLE
//     // Configure dynamic frequency scaling:
//     // maximum and minimum frequencies are set in sdkconfig,
//     // automatic light sleep is enabled if tickless idle support is enabled.
//     esp_pm_config_t pm_config = {
//         .max_freq_mhz = 80,
//         .min_freq_mhz = 10,
// #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
//         .light_sleep_enable = true
// #endif
//     };
//     ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
// #endif // CONFIG PM ENABLE

    // 初始化NVS存储区
    ESP_ERROR_CHECK(nvs_flash_init());

    // Wi-Fi初始化
    ESP_LOGI(TAG, "Wi-Fi initialization");
    wifi_initialize();

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_station_initialize();

    mqtt_app_start();
}
