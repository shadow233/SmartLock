file(TO_CMAKE_PATH "$ENV{IDF_PATH}" IDF_PATH)  # 将Windows下ESP-IDF的路径转化CMAKE路径

idf_component_register(SRCS "MG90S.c"
                    INCLUDE_DIRS "include"
                    "${IDF_PATH}/components/driver/gpio/include"
                    "${IDF_PATH}/components/driver/ledc/include"
                    "${IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos"
                    )
