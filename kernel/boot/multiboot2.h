#pragma once

#include "boot/bootinfo.h"
#include "klib/types.h"

#define MB2_TAG_TYPE_END                    0
#define MB2_TAG_TYPE_MEMMAP                 6

#define MB2_MEMORY_AVAILABLE                1
#define MB2_MEMORY_RESERVED                 2

struct Multiboot2_Tag {
    u32 type;
    u32 size;
};

struct Multiboot2_TagString {
    u32 type;
    u32 size;
    string str;
};

struct Multiboot2_MemmapEntry {
    u64 address;
    u64 length;
    u32 type;
    u32 zero;
};

struct Multiboot2_TagMemmap {
    u32 type;
    u32 size;
    u32 entry_size;
    u32 entry_version;
};

struct Multiboot2_Info {
    u32 size;
    u32 reserved;
};

void multiboot2_parse(u32 address, struct BootInfo* bootinfo);