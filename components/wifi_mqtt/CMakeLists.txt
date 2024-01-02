file(TO_CMAKE_PATH "$ENV{IDF_PATH}" IDF_PATH)  # 将Windows下ESP-IDF的路径转化CMAKE路径

idf_component_register(SRCS "wifi_mqtt.c"
                    INCLUDE_DIRS "include"
                    "${IDF_PATH}/components/esp_wifi/include"
                    "${IDF_PATH}/components/nvs_flash/include"
                    "${IDF_PATH}/components/esp_partition/include"
                    "${IDF_PATH}/components/esp_netif/include"
                    "${IDF_PATH}/components/esp_event/include"
                    "${IDF_PATH}/components/esp_pm/include"
                    "${IDF_PATH}/components/mqtt/esp-mqtt/include"
                    "${IDF_PATH}/components/tcp_transport/include"
                    )
