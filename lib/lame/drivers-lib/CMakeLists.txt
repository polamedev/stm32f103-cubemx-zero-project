
add_library(lame-drivers STATIC)
add_library(lame::drivers ALIAS lame-drivers)

target_include_directories(lame-drivers
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc/lame
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

target_sources(lame-drivers
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/Pin.c
    ${CMAKE_CURRENT_LIST_DIR}/src/mSec.c
)

target_link_libraries(lame-drivers
    st_hal
    lame::soft
)
