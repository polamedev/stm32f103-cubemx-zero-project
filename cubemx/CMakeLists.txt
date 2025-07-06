cmake_minimum_required(VERSION 3.22)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

option(USE_CUBEMX_CMSIS "Использовать CMSIS, сгенерированный CubeMX" ON)
option(USE_CUBEMX_ST_HAL_CONFIG "Использовать файл hal_conf, сгенерированный CubeMX" ON)
option(USE_CUBEMX_ST_HAL "Использовать ST_HAL, сгенерированный CubeMX" ON)


# Директория генерации cubemx
set(cubemx_gen_dir ${CMAKE_CURRENT_SOURCE_DIR}/gen)

# Установим эту переменную в GLOBAL PROPERTY, нужно чтобы можно было воспользоваться этим свойством в функции cubemx_get_st_hal_files
# из любого места, а не только из текущего файла 
set_property(GLOBAL PROPERTY cubemx_gen_dir_property "${cubemx_gen_dir}")

# ------------------------------------------------------------------------------------------------------------
# Функция cubemx_get_st_hal_files получает список необходимых файлов .c библиотеки ST_HAL, сгенерированным CubeMX
function(cubemx_get_st_hal_files varName)
    # Получить путь директории генерации CubeMX
    get_property(cubemx_gen_dir GLOBAL PROPERTY cubemx_gen_dir_property)

    # Поиск строк, содержащих файлы с hal драйвером, в cmake файле, сгенерированном cubemx
    file(STRINGS ${cubemx_gen_dir}/cmake/stm32cubemx/CMakeLists.txt cubemx_hal_file_cmake_string REGEX "Drivers/STM32F1xx_HAL_Driver/Src/")
    # Разделим полученные 
    string(REPLACE "    \${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/" ";" cubemx_hal_file_list ${cubemx_hal_file_cmake_string})
    # Первый элемент в списке файлов пустой, его нужно убрать 
    list(REMOVE_AT cubemx_hal_file_list 0)

    # Установить список файлов в имя переданной переменной с помощью PARENT_SCOPE
    set(${varName} "${cubemx_hal_file_list}" PARENT_SCOPE)

endfunction()


# --------------------------------------------------------------------------------------------------
# # Создание библиотеки cmsis если задана опция USE_CUBEMX_CMSIS

if(USE_CUBEMX_CMSIS)
    message(STATUS "USE_CUBEMX_CMSIS")
    set(CMSIS_DIR ${cubemx_gen_dir}/Drivers/CMSIS)
    # Подключаем файл по созданию таргета из библиотеки lame_cmakelib
    include(${LAME_CMAKELIB}/target/lib_cmsis.cmake)
endif()


# --------------------------------------------------------------------------------------------------
# Создание библиотеки для конфигурации st HAL
if(USE_CUBEMX_ST_HAL_CONFIG)
    message(STATUS "USE_CUBEMX_ST_HAL_CONFIG")
    # Копируем файл в каталог сборки, чтобы отделить его от файла main.h, генерируемого cubeMX, нужно чтобы случайно не включить main.h
    configure_file(${cubemx_gen_dir}/Core/Inc/stm32f1xx_hal_conf.h ${CMAKE_CURRENT_BINARY_DIR}/cubemx_st_hal_config/stm32f1xx_hal_conf.h COPYONLY)
    set(ST_HAL_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/cubemx_st_hal_config)
    # Подключаем файл по созданию таргета из библиотеки lame_cmakelib
    include(${LAME_CMAKELIB}/target/lib_st_hal_config.cmake)
endif()


# --------------------------------------------------------------------------------------------------
# Создание библиотеки hal

if(USE_CUBEMX_ST_HAL)
    message(STATUS "USE_CUBEMX_CMSIS")
    # Установим директорию до исходников hal
    set(ST_HAL_DIR ${cubemx_gen_dir}/Drivers/STM32F1xx_HAL_Driver)

    # Установка семейства процессора в HAL
    set(ST_MCU_NAME STM32F103xB)

    # Установим опцию использования спика файлов, требуемых для компиляции cubemx
    set(USE_CUBEMX_ST_HAL_SRC_LIST ON)
    # Подключаем файл по созданию таргета из библиотеки lame_cmakelib
    include(${LAME_CMAKELIB}/target/lib_st_hal.cmake)
endif()

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
