cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C ASM)
# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	USE_HAL_DRIVER 
	STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

set(stm32cubemx_gen_dir ${CMAKE_CURRENT_SOURCE_DIR}/gen)

# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}
    
    ${stm32cubemx_gen_dir}/Core/Inc
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${stm32cubemx_gen_dir}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${stm32cubemx_gen_dir}/Drivers/CMSIS/Include
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32cubemx.h

    ${stm32cubemx_gen_dir}/Core/Src/main.c
    ${stm32cubemx_gen_dir}/Core/Src/stm32f1xx_it.c
    ${stm32cubemx_gen_dir}/Core/Src/stm32f1xx_hal_msp.c
    ${stm32cubemx_gen_dir}/Core/Src/sysmem.c
    ${stm32cubemx_gen_dir}/Core/Src/syscalls.c
    ${stm32cubemx_gen_dir}/startup_stm32f103xb.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${stm32cubemx_gen_dir}/Core/Src/system_stm32f1xx.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    ${stm32cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
)

# Drivers Midllewares



# Link directories setup
set(MX_LINK_DIRS

)
# Project static libraries
set(MX_LINK_LIBS 
    STM32_Drivers
    
)
# Interface library for includes and symbols
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)


# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
