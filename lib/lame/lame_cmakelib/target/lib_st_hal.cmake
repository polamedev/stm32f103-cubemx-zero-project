# Создание библиотеки hal

option(USE_CUBEMX_ST_HAL_SRC_LIST "Использовать требуемый список файлов библиотеки, сгенерированный CubeMX" OFF)
set(ST_MCU_NAME STM32F103xB CACHE STRING "Тип процесса в HAL")

# Установка пути до исходников, нужно чтобы можно было задать любое расположение с помощью переменных
if(NOT DEFINED ST_HAL_DIR)
    set(ST_HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Создание цели библиотеки
# Ключевое слово STATIC - говорит о создание статической библиотеке
add_library(st_hal STATIC)

set(HAL_Defines_Syms
    USE_HAL_DRIVER
    ${ST_MCU_NAME}
)


set(HAL_Include_Dirs
    ${ST_HAL_DIR}/Inc/Legacy
    ${ST_HAL_DIR}/Inc
)

if(USE_CUBEMX_ST_HAL_SRC_LIST)

    cubemx_get_st_hal_files(cubemx_hal_file_list)
    list(LENGTH cubemx_hal_file_list cubemx_hal_file_list_size)
    message(STATUS "st_hal_files[${cubemx_hal_file_list_size}]: " "${cubemx_hal_file_list}")

    # Дополним полученный список файлов действительными путями до них
    list(TRANSFORM cubemx_hal_file_list PREPEND "${ST_HAL_DIR}/Src/")
    set(HAL_Src
        ${cubemx_hal_file_list}
    )

else()
    message(SEND_ERROR "USE_CUBEMX_ST_HAL_SRC_LIST ON - пока не поддерживается")
endif()

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
