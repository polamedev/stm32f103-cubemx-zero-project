#include "opaque.h"
#include "assert.h"

typedef struct opaque_impl {
    int ch;
    int ch2;
} opaque_impl;

static_assert(sizeof(opaque_impl) == sizeof(opaque_t), "sizeof");
static_assert(alignof(opaque_impl) == alignof(opaque_t), "alignof");

void opaque_task(opaque_t *op)
{
    opaque_impl *impl = (opaque_impl *)op;

    if (impl->ch == 1) {
        impl->ch = 0;
    }
}
