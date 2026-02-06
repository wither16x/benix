#include "mm/mapping.h"
#include "memory.h"
#include "mm/paging.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "klib/asm.h"

void setup_paging(void) {
    struct PageDirectory* kpd = get_kernel_page_table();
    enable_paging((u32)kpd);
}

bool is_paging_enabled(void) {
    u32 cr0;
    ASM("mov %%cr0, %0" : "=r"(cr0));
    return cr0 & (1u << 31);
}

void map_low_memory(void) {
    for (u32 address = 0; address < ADDR_KERNEL; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW);
    }
}

void map_kernel(void) {
    for (u32 address = ADDR_KERNEL; address < ADDR_KERNEL_STACK; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW);
    }

    for (u32 address = ADDR_KERNEL_STACK; address < ADDR_KERNEL_STACK + PAGE_SIZE * 8; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW);
    }
}

void map_userspace(void) {
    for (u32 address = ADDR_ARGC_ARGV; address < ADDR_ARGC_ARGV + PAGE_SIZE; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW | PAGE_USER);
    }

    for (u32 address = ADDR_SHELL; address < ADDR_USER_STACK; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW | PAGE_USER);
    }

    for (u32 address = ADDR_USER_STACK; address < ADDR_USER_STACK + PAGE_SIZE * 8; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW | PAGE_USER);
    }
}

void map_bitmap(void) {
    u32 frames = get_pmm()->total_frames;
    u32 bitmap_bytes = (frames + 7) / 8;
    u32 bitmap_pages = (bitmap_bytes + PAGE_SIZE - 1) / PAGE_SIZE;

    for (u32 address = ADDR_BITMAP_FRAMES; address < ADDR_BITMAP_FRAMES + bitmap_pages * PAGE_SIZE; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW);
    }
}