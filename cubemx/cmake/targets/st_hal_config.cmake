# Пользовательская библиотека для конфигурации st HAL, содержит только путь включения до файла
# с настройками stm32f1xx_hal_conf.h. Такой способ сделан по аналоги с freertos

if(NOT DEFINED ST_HAL_CONFIG_DIR)
    set(ST_HAL_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_library(st_hal_config INTERFACE)

target_include_directories(st_hal_config INTERFACE ${ST_HAL_CONFIG_DIR})

