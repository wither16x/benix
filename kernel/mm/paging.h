#pragma once

#include "klib/types.h"

#define PAGE_SIZE                   0x1000

#define PAGE_PRESENT                0x1
#define PAGE_RW                     0x2     // RW = Read/Write
#define PAGE_USER                   0x4

#define PAGE_ADDRESS_MASK           0xfffff000

void enable_paging(u32 pd);