# --------------------------------------------------------------------------------------------------
# Создание библиотеки cmsis

if(NOT DEFINED CMSIS_DIR)
    set(CMSIS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# На данный строчке создается цель библиотеки (название), но в ней нет еще файлов
# Ключевое слово INTERFACE, говорит что это библиотека интерфейсов, 
# которая содержит только header файлы и макроопределения, и их не нужно сбирать
add_library(cmsis INTERFACE)

set(CMSIS_Include_Dirs
    ${CMSIS_DIR}/Device/ST/STM32F1xx/Include
    ${CMSIS_DIR}/Include
)

# Добавляем пути включений и макроопоределения в библиотеку
# Ключевое слово INTERFACE сообщает CMake, что файлы должны быть доступны в виде интерфейса 
# библиотеки, то есть они должны быть видны при использовании библиотеки
target_include_directories(cmsis INTERFACE ${CMSIS_Include_Dirs})
