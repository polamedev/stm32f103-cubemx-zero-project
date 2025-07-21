
add_library(lame-soft STATIC)
add_library(lame::soft ALIAS lame-soft)

set(LAME_C_INC_DIR inc)
set(LAME_C_SRC_DIR src)

target_include_directories(lame-soft
    INTERFACE
    ${LAME_C_INC_DIR}

    PRIVATE
    ${LAME_C_INC_DIR}/lame
)

target_sources(lame-soft
    PRIVATE
    ${LAME_C_SRC_DIR}/Event.c
    ${LAME_C_SRC_DIR}/SoftTimer.c
)

# TODO lame-soft не должна использовать lame-drivers
target_link_libraries(lame-soft
    lame-drivers
)