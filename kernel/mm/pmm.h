#pragma once

#include "klib/types.h"

#define FRAME_SIZE                  0x1000

#define BYTE(b)                     ((b) / 8)
#define MASK(b)                     (1 << ((b) % 8))

struct PMM {
    u8* bitmap;
    u64 total_frames;

    u64 (*alloc)(void);
    void (*free)(u64 frame);
};

void init_pmm(u64 total_memory, u8* bitmap);
struct PMM* get_pmm(void);