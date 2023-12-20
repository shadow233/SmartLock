file(TO_CMAKE_PATH "$ENV{IDF_PATH}" IDF_PATH)  # 将Windows下ESP-IDF的路径转化CMAKE路径

idf_component_register(SRCS "led.c"
                    INCLUDE_DIRS "include"
                    # 添加官方gpio组件
                    "${IDF_PATH}/components/driver/gpio/include"
                    )
