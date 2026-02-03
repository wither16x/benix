#include "mm/mapping.h"
#include "memory.h"
#include "mm/paging.h"
#include "mm/vmm.h"

void setup_paging(void) {
    struct PageDirectory* kpd = get_kernel_page_table();
    enable_paging((u32)kpd);
}

void map_kernel(void) {
    for (u32 address = 0; address < ADDR_ARGC_ARGV; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW);
    }
}

void map_userspace(void) {
    for (u32 address = ADDR_ARGC_ARGV; address < ADDR_USER_STACK + PAGE_SIZE; address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW | PAGE_USER);
    }
}

void map_bitmap(void) {
    for (u32 address = ADDR_BITMAP_FRAMES; address < ADDR_BITMAP_FRAMES + (1024 * 1024); address += PAGE_SIZE) {
        get_vmm()->map(address, address, PAGE_PRESENT | PAGE_RW);
    }
}