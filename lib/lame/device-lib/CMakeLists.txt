
add_library(lame-device INTERFACE)
add_library(lame::device ALIAS lame-device)

target_include_directories(lame-device INTERFACE ${CMAKE_CURRENT_LIST_DIR}/inc)

target_link_libraries(lame-device
    INTERFACE
    lame-config
    lame::drivers
)

# LED
add_library(led STATIC)
add_library(lame::led ALIAS led)
target_sources(led PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/Led.c)
target_link_libraries(led PUBLIC lame::device)
target_include_directories(led
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc/lame
)
