cmake_minimum_required(VERSION 3.22)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Директория генерации cubemx
set(cubemx_gen_dir ${CMAKE_CURRENT_SOURCE_DIR}/gen)


# --------------------------------------------------------------------------------------------------
# Создание библиотеки cmsis

# На данный строчке создается цель библиотеки (название), но в ней нет еще файлов
# Ключевое слово INTERFACE, говорит что это библиотека интерфейсов, 
# которая содержит только header файлы и макроопределения, и их не нужно сбирать
add_library(cmsis INTERFACE)

set(CMSIS_Include_Dirs
    ${cubemx_gen_dir}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${cubemx_gen_dir}/Drivers/CMSIS/Include
)

# Добавляем пути включений и макроопоределения в библиотеку
# Ключевое слово INTERFACE сообщает CMake, что файлы должны быть доступны в виде интерфейса 
# библиотеки, то есть они должны быть видны при использовании библиотеки
target_include_directories(cmsis INTERFACE ${CMSIS_Include_Dirs}) 


# --------------------------------------------------------------------------------------------------

# Пользовательская библиотека для конфигурации st HAL, содержит только путь включения до файла
# с настройками stm32f1xx_hal_conf.h. Такой способ сделан по аналоги с freertos

add_library(st_hal_config INTERFACE)

target_include_directories(st_hal_config SYSTEM
  INTERFACE
    ${cubemx_gen_dir}/Core/Inc
)


# --------------------------------------------------------------------------------------------------
# Создание библиотеки hal

# Создание цели библиотеки
# Ключевое слово OBJECT - говорит о создание библиотеки объектных файлов без архивации и линковки
add_library(st_hal OBJECT)

set(HAL_Defines_Syms 
    USE_HAL_DRIVER 
    STM32F103xB
)

set(HAL_Include_Dirs
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc
)

set(HAL_Src
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
)

target_compile_definitions(st_hal PUBLIC ${HAL_Defines_Syms})
target_include_directories(st_hal PUBLIC ${HAL_Include_Dirs})

# Перечисляем файл для компиляции библиотеки
# Ключевое слово PRIVATE говорит, что файлы нужны только для сборки данной цели
target_sources(st_hal PRIVATE ${HAL_Src})

# Подключаем библиотеки 
# Ключевое слово PUBLIC говорит, что библиотека нужна для сборки цели и для интерфейса данной цели
target_link_libraries(st_hal PUBLIC cmsis st_hal_config)


# --------------------------------------------------------------------------------------------------
# Создание объектной библиотеки CubeMX

# Создание цели библиотеки
# Ключевое слово OBJECT - говорит о создание библиотеки объектных файлов без архивации и линковки
add_library(cubemx OBJECT)


set(CUBEMX_Include_Dirs
    ${cubemx_gen_dir}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR} 
)


set(CUBEMX_Src
    ${cubemx_gen_dir}/Core/Src/main.c
    ${cubemx_gen_dir}/Core/Src/stm32f1xx_it.c
    ${cubemx_gen_dir}/Core/Src/stm32f1xx_hal_msp.c
    ${cubemx_gen_dir}/Core/Src/sysmem.c
    ${cubemx_gen_dir}/Core/Src/syscalls.c
    ${cubemx_gen_dir}/Core/Src/system_stm32f1xx.c
    ${cubemx_gen_dir}/startup_stm32f103xb.s

    ${CMAKE_CURRENT_SOURCE_DIR}/stm32cubemx.h
)

target_include_directories(cubemx PUBLIC ${CUBEMX_Include_Dirs})
target_sources(cubemx PRIVATE ${CUBEMX_Src})
target_link_libraries(cubemx PRIVATE st_hal)
