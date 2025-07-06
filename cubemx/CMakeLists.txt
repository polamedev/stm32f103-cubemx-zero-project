cmake_minimum_required(VERSION 3.22)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Директория генерации cubemx
set(cubemx_gen_dir ${CMAKE_CURRENT_SOURCE_DIR}/gen)

# Установим эту переменную в GLOBAL PROPERTY, нужно чтобы можно было воспользоваться этим свойством в функции cubemx_get_st_hal_files
# из любого места, а не только из текущего файла 
set_property(GLOBAL PROPERTY cubemx_gen_dir_property "${cubemx_gen_dir}")


include(cmake/cubemx_get_st_hal_files.cmake)


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

# Копируем файл в каталог сборки, чтобы отделить его от файла main.h, генерируемого cubeMX, нужно чтобы случайно не включить main.h
configure_file(${cubemx_gen_dir}/Core/Inc/stm32f1xx_hal_conf.h ${CMAKE_CURRENT_BINARY_DIR}/cubemx_hal_conf/stm32f1xx_hal_conf.h COPYONLY)

# Пользовательская библиотека для конфигурации st HAL, содержит только путь включения до файла
# с настройками stm32f1xx_hal_conf.h. Такой способ сделан по аналоги с freertos

add_library(st_hal_config INTERFACE)

target_include_directories(st_hal_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/cubemx_hal_conf)


# --------------------------------------------------------------------------------------------------
# Создание библиотеки hal

# Создание цели библиотеки
# Ключевое слово STATIC - говорит о создание статической библиотеке
add_library(st_hal STATIC)

set(HAL_Defines_Syms
    USE_HAL_DRIVER
    STM32F103xB
)


set(HAL_Include_Dirs
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Inc
)

cubemx_get_st_hal_files(cubemx_hal_file_list)
list(LENGTH cubemx_hal_file_list cubemx_hal_file_list_size)
message(STATUS "st_hal_files[${cubemx_hal_file_list_size}]: " "${cubemx_hal_file_list}")

# Дополним полученный список файлов действительными путями до них
list(TRANSFORM cubemx_hal_file_list PREPEND "${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver/Src/")
set(HAL_Src
    ${cubemx_hal_file_list}
)

target_compile_definitions(st_hal PUBLIC ${HAL_Defines_Syms})
target_include_directories(st_hal PUBLIC ${HAL_Include_Dirs})

# Перечисляем файл для компиляции библиотеки
# Ключевое слово PRIVATE говорит, что файлы нужны только для сборки данной цели
target_sources(st_hal PRIVATE ${HAL_Src})

# Подключаем библиотеки 
# Ключевое слово PUBLIC говорит, что библиотека нужна для сборки цели и для интерфейса данной цели
target_link_libraries(st_hal
    PUBLIC
    cmsis
    st_hal_config
)

# --------------------------------------------------------------------------------------------------
# Создание объектной библиотеки CubeMX

# Создание цели библиотеки
# Ключевое слово OBJECT - говорит о создание библиотеки объектных файлов без архивации и линковки
add_library(cubemx OBJECT)

set(CUBEMX_Src
    ${cubemx_gen_dir}/Core/Src/main.c
    ${cubemx_gen_dir}/Core/Src/stm32f1xx_it.c
    ${cubemx_gen_dir}/Core/Src/stm32f1xx_hal_msp.c
    ${cubemx_gen_dir}/Core/Src/sysmem.c
    ${cubemx_gen_dir}/Core/Src/syscalls.c
    ${cubemx_gen_dir}/Core/Src/system_stm32f1xx.c
    ${cubemx_gen_dir}/startup_stm32f103xb.s

    ${CMAKE_CURRENT_SOURCE_DIR}/cubemx.h
)

target_include_directories(cubemx
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}

    PRIVATE
    ${cubemx_gen_dir}/Core/Inc
)
target_sources(cubemx PRIVATE ${CUBEMX_Src})

# PRIVATE нужен так как подключение cubemx само по себе не подключит библиотеку st_hal, ее
# ее нужно включать для каждого таргета, где будет использован cubemx
# Сейчас используется PUBLIC, так как теперь st_hal - статическая библиотека, и она будет линковаться при 
# подключении объектной библиотеки cubemx (https://stackoverflow.com/questions/75339783/how-can-i-link-a-cmake-object-library-to-another-cmake-object-library)
target_link_libraries(cubemx PUBLIC st_hal)
