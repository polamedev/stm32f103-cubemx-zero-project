#pragma once

#include <stdalign.h>
#include <stddef.h>


typedef struct opaque_t
{
    alignas(4) char b[8];
} opaque_t;