
add_library(lame-c STATIC)

set(LAME_C_INC_DIR inc)
set(LAME_C_SRC_DIR src)

target_include_directories(lame-c
    INTERFACE
    ${LAME_C_INC_DIR}

    PRIVATE
    ${LAME_C_INC_DIR}/lame
)

target_sources(lame-c
    PRIVATE
    ${LAME_C_SRC_DIR}/event.c
)
