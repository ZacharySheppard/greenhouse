idf_component_register(
                    SRCS "main.cpp" "gpio.cpp" "device.cpp"                    
                    PRIV_REQUIRES spi_flash
                    PRIV_REQUIRES esp_driver_gpio
                    INCLUDE_DIRS ".")
