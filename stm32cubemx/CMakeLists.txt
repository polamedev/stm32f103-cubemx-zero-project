cmake_minimum_required(VERSION 3.22)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	USE_HAL_DRIVER 
	STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

# Директория генерации cubemx
set(cubemx_gen_dir ${CMAKE_CURRENT_SOURCE_DIR}/gen)

# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}
    
    ${cubemx_gen_dir}/Core/Inc
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${cubemx_gen_dir}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${cubemx_gen_dir}/Drivers/CMSIS/Include
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32cubemx.h

    ${cubemx_gen_dir}/Core/Src/main.c
    ${cubemx_gen_dir}/Core/Src/stm32f1xx_it.c
    ${cubemx_gen_dir}/Core/Src/stm32f1xx_hal_msp.c
    ${cubemx_gen_dir}/Core/Src/sysmem.c
    ${cubemx_gen_dir}/Core/Src/syscalls.c
    ${cubemx_gen_dir}/startup_stm32f103xb.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${cubemx_gen_dir}/Core/Src/system_stm32f1xx.c
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

# Drivers Midllewares



# Link directories setup
set(MX_LINK_DIRS

)
# Project static libraries
set(MX_LINK_LIBS 
    STM32_Drivers
    
)

# ------------------------------------------------------------------------------------------------
# Создание библиотеки stm32cubemx

# На данный строчке создается цель библиотеки (название), но в ней нет еще файлов
# Ключевое слово INTERFACE, говорит что это библиотека интерфейсов, 
# которая содержит только header файлы и макроопределения, и их не нужно сбирать
add_library(stm32cubemx INTERFACE)

# Добавляем пути включений и макроопоределения в библиотеку
# Ключевое слово INTERFACE сообщает CMake, что файлы должны быть доступны в виде интерфейса 
# библиотеки, то есть они должны быть видны при использовании библиотеки
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs}) 
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# -------------------------------------------------------------------------------------------------

# Создание библиотеки STM32_Drivers

# Создание цели библиотеки
# Ключевое слово OBJECT - говорит о создание библиотеки объектных файлов без архивации и линковки
add_library(STM32_Drivers OBJECT)

# Перечисляем файл для компиляции библиотеки
# Ключевое слово PRIVATE говорит, что файлы нужны только для сборки данной цели
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})

# Подключаем библиотеки 
# Ключевое слово PUBLIC говорит, что библиотека нужна для сборки цели и для интерфейса данной цели
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)

# --------------------------------------------------------------------------------------------------
# Создание библиотеки генерированной CubeMX

add_library(cubemx OBJECT)
target_sources(cubemx PRIVATE ${MX_Application_Src})
target_link_libraries(cubemx PUBLIC stm32cubemx)

# Link directories setup
# target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})
