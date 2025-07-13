cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME stm32f103-cubemx-zero-project)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Подключить библиотеку lame_cmakelib
add_subdirectory(lib/lame/lame_cmakelib)
message(STATUS "LAME_CMAKELIB" ${LAME_CMAKELIB})

# Add STM32CubeMX generated sources

# set(USE_CUBEMX_CMSIS OFF) # Использовать CMSIS, сгенерированный CubeMX
# set(USE_CUBEMX_ST_HAL_CONFIG OFF) # Использовать файл hal_conf, сгенерированный CubeMX
# set(USE_CUBEMX_ST_HAL OFF) # Использовать файл hal_conf, сгенерированный CubeMX
add_subdirectory(cubemx)

# При использовании USE_CUBEMX_CMSIS OFF необходимо самостоятельно подключить библиотеку CMSIS
# add_subdirectory(lib/drivers/CMSIS)

# При использовании USE_CUBEMX_ST_HAL_CONFIG OFF необходимо самостоятельно предоставить библиотеку с инклудом hal_conf
# add_library(st_hal_config INTERFACE)
# target_include_directories(st_hal_config INTERFACE config)

# При использовании USE_CUBEMX_ST_HAL OFF необходимо самостоятельно подключить библиотеку с st_hal
# set(USE_CUBEMX_ST_HAL_SRC_LIST ON) # Установим опцию использования спика файлов, требуемых для компиляции cubemx
# add_subdirectory(lib/drivers/STM32F1xx_HAL_Driver)

add_subdirectory(lib/lame/lame-c)

# include(lib/tinyusb/hw/bsp/family_support.cmake)
# family_add_tinyusb(${TARGET} OPT_MCU_STM32F1 OFF)


add_library(tinyusb_config INTERFACE)
target_include_directories(tinyusb_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/app)
target_compile_definitions(tinyusb_config INTERFACE CFG_TUSB_MCU=OPT_MCU_STM32F1)
target_compile_definitions(tinyusb_config INTERFACE STM32F103xB)

add_subdirectory(lib/tinyusb/src)
# target_sources(tinyusb PUBLIC
# )


# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    app/main.c
    app/interrupts.c

    lib/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c

)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    config
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    cubemx
    # st_hal
    lame-c
    tinyusb
)

# Флаги линковки могут быть специфичны для компиляторы, наверное, стоит вынести отдельный файл
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-T${CMAKE_SOURCE_DIR}/cubemx/gen/STM32F103XX_FLASH.ld")
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
