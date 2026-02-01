#include "boot/multiboot2.h"
#include "boot/bootinfo.h"
#include "klib/bool.h"

void multiboot2_parse(u32 address, struct BootInfo* bootinfo) {
    struct Multiboot2_Info* mb2 = (void*)address;
    u8* p = (u8*)address + sizeof(struct Multiboot2_Info);
    struct Multiboot2_Tag* tag;
    struct Multiboot2_TagMemmap* memmap;
    struct Multiboot2_MemmapEntry* e;
    u8* pentry;
    u8* memmap_end;
    i32 i = 0;
    u64 total_memory;

    while (true) {
        tag = (struct Multiboot2_Tag*)p;

        if (tag->type == MB2_TAG_TYPE_END)
            break;

        if (tag->type == MB2_TAG_TYPE_MEMMAP) {
            memmap = (struct Multiboot2_TagMemmap*)tag;

            if (memmap->entry_size < sizeof(struct Multiboot2_MemmapEntry))
                return;

            pentry = p + sizeof(struct Multiboot2_TagMemmap);
            memmap_end = p + memmap->size;
            while (pentry + memmap->entry_size <= memmap_end) {

                e = (struct Multiboot2_MemmapEntry*)pentry;

                if (e->type == MB2_MEMORY_AVAILABLE) {
                    if (bootinfo->memmap && bootinfo->memmap->entries) {
                        bootinfo->memmap->entries[i] = (struct MemoryMapEntry){e->address, e->length};
                        i++;
                        total_memory += e->length;
                    }
                }
                pentry += memmap->entry_size;
            }
            bootinfo->memmap->count = i;
        }

        p += tag->size;
        p = (u8*)(((u32)p + 7) & ~7);
    }

    bootinfo->total_memory = total_memory;
}