
file(TO_CMAKE_PATH "$ENV{IDF_PATH}" IDF_PATH)  # 将Windows下ESP-IDF的路径转化CMAKE路径

idf_component_register(SRCS "FPM383C.c"
                    INCLUDE_DIRS "include"
                    "${IDF_PATH}/components/driver/gpio/include"
                    "${IDF_PATH}/components/driver/uart/include"
                    "${IDF_PATH}/components/esp_ringbuf/include"               
                    )
