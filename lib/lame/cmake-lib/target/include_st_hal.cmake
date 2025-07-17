# Создание библиотеки hal

set(ST_HAL_MCU_NAME STM32F103xB CACHE STRING "Тип процесса в HAL")

# Установка пути до исходников, нужно чтобы можно было задать любое расположение с помощью переменных
set(ST_HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Путь до исходников ST_HAL")

option(ST_HAL_USE_FULL_SOURCES "Использовать полный список файлов для сборки" ON)

# Функция по установки файлов исходников учитывая ST_HAL_DIR
function(st_hal_sources file_list)

    list(TRANSFORM file_list PREPEND "${ST_HAL_DIR}/Src/")
    set(HAL_Src
        ${HAL_Src}
        ${file_list}
    )

    target_sources(st_hal PRIVATE ${HAL_Src})

endfunction()


# Создание цели библиотеки
# Ключевое слово STATIC - говорит о создание статической библиотеке
add_library(st_hal STATIC)

set(HAL_Defines_Syms
    USE_HAL_DRIVER
    ${ST_HAL_MCU_NAME}
)


set(HAL_Include_Dirs
    ${ST_HAL_DIR}/Inc/Legacy
    ${ST_HAL_DIR}/Inc
    ${CMAKE_CURRENT_LIST_DIR}/../include/st_hal
)

message(STATUS "HAL_INC ${HAL_Include_Dirs}")

if(ST_HAL_USE_FULL_SOURCES)
    message(SEND_ERROR "ST_HAL_USE_FULL_SOURCES - пока не поддерживается")
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
