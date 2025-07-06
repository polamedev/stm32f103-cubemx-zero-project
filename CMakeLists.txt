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



# Add STM32CubeMX generated sources
option(USE_CUBEMX_CMSIS "Использовать CMSIS, сгенерированный CubeMX" ON)
# option(USE_CUBEMX_HAL_CONFIG "Использовать файл hal_conf, сгенерированный CubeMX" OFF)

add_subdirectory(cubemx)

# При использовании USE_CUBEMX_CMSIS OFF необходимо самостоятельно подключить библиотеку CMSIS
# add_subdirectory(lib/drivers/CMSIS)

# При использовании USE_CUBEMX_HAL_CONFIG OFF необходимо самостоятельно предоставить библиотеку с инклудом hal_conf
# add_library(st_hal_config INTERFACE)
# target_include_directories(st_hal_config INTERFACE app)




# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    app/main.c
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
)

# Флаги линковки могут быть специфичны для компиляторы, наверное, стоит вынести отдельный файл
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-T${CMAKE_SOURCE_DIR}/cubemx/gen/STM32F103XX_FLASH.ld")
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
