
add_library(lame-hw STATIC)

target_include_directories(lame-hw
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}

    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers
)

target_sources(lame-hw
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers/led_driver.c
)

target_link_libraries(lame-hw
    st_hal
)
