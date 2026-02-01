#include "boot/bootinfo.h"
#include "klib/null.h"

static struct BootInfo bootinfo;
static struct MemoryMap memmap;
static struct MemoryMapEntry memmap_entries[MEMMAP_MAX_ENTRIES];

void init_bootinfo(void) {
    bootinfo.memmap = &memmap;
    bootinfo.memmap->entries = memmap_entries;
    memmap.count = 0;
    memmap.capacity = MEMMAP_MAX_ENTRIES;
}

struct BootInfo* get_bootinfo(void) {
    return &bootinfo;
}