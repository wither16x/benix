#pragma once

#include "klib/types.h"

#define PAGE_ENTRIES                1024

typedef u32 page_entry;

struct __attribute__((aligned(0x1000))) PageTable {
    page_entry entries[PAGE_ENTRIES];
};

struct __attribute__((aligned(0x1000))) PageDirectory {
    page_entry entries[PAGE_ENTRIES];
};

struct VMM {
    struct PageTable* (*alloc_table)(void);
    void (*map)(u32 virt, u32 phys, u32 flags);
};

void init_vmm(void);
struct VMM* get_vmm(void);
struct PageDirectory* get_kernel_page_table(void);