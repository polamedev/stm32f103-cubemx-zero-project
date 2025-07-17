
add_library(lame-device STATIC)

target_include_directories(lame-device
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_sources(lame-device
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/led.c
)

target_link_libraries(lame-device
    lame-drivers
)
