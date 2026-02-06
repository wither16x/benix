#pragma once

#include "klib/types.h"

struct Heap {
    u8* position;
    u32 top;

    voidptr (*alloc)(size_t size);
    void (*free)(voidptr ptr);
};

void init_heap(void);
struct Heap* get_heap(void);