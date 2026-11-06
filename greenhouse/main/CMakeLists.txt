idf_component_register(
                    SRCS "main.cpp" "requests.cpp" "socket.cpp" "gpio.cpp" "device.cpp" "wifi.cpp"
                    PRIV_REQUIRES spi_flash esp_wifi nvs_flash esp-tls mbedtls esp_netif
                    PRIV_REQUIRES esp_driver_gpio
                    INCLUDE_DIRS ".")


set(ABS_CONFIG_PATH "\"${CMAKE_SOURCE_DIR}/.env.local\"")
target_compile_definitions(${COMPONENT_LIB} PRIVATE MY_CONFIG_PATH=${ABS_CONFIG_PATH})
