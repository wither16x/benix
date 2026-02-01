#pragma once

#include "klib/types.h"

#define MEMMAP_MAX_ENTRIES 256

struct MemoryMapEntry {
    u64 address;
    u64 length;
};

struct MemoryMap {
    struct MemoryMapEntry* entries;
    u32 count;
    u32 capacity;
};

struct BootInfo {
    struct MemoryMap* memmap;
    u64 total_memory;
};

void init_bootinfo(void);
struct BootInfo* get_bootinfo(void);