# Пользовательская библиотека для конфигурации st HAL, содержит только путь включения до файла
# с настройками stm32f1xx_hal_conf.h. Такой способ сделан по аналоги с freertos

add_library(st_hal_config INTERFACE)

target_include_directories(st_hal_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/cubemx_hal_config)

