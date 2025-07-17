
add_library(lame-hw STATIC)

target_include_directories(lame-hw
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}

    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers
)

target_sources(lame-hw
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/device/led.c
    ${CMAKE_CURRENT_LIST_DIR}/drivers/pin_driver.c
)

target_link_libraries(lame-hw
    st_hal
)
