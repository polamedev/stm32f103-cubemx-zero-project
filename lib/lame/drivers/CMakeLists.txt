
add_library(lame-drivers STATIC)

target_include_directories(lame-drivers
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}

    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers
)

target_sources(lame-drivers
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/pin_driver.c
)

target_link_libraries(lame-drivers
    st_hal
)
