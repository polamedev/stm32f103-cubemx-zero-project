
add_library(lame-device STATIC)
add_library(lame::device ALIAS lame-device)

target_include_directories(lame-device
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

target_sources(lame-device
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/led.c
)

target_link_libraries(lame-device
    lame::drivers
)
