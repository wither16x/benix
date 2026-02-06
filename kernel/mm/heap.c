#include "mm/heap.h"
#include "memory.h"
#include "klib/null.h"
#include "mm/paging.h"
#include "mm/pmm.h"
#include "mm/vmm.h"

static struct Heap heap;

static voidptr alloc(size_t size) {
    u32 virt = heap.top;
    u64 frame;

    size = (size + 0xfff) & ~0xfff;

    if (heap.top + size > ADDR_KERNEL_HEAP_END) {
        return NULL;
    }

    for (u32 offset = 0; offset < size; offset += PAGE_SIZE) {
        frame = get_pmm()->alloc();
        if (frame == -1) {
            return NULL;
        }
        get_vmm()->map(virt + offset, frame * PAGE_SIZE, PAGE_PRESENT | PAGE_RW);
    }

    heap.top += size;
    
    return (voidptr)virt;
}

static void free(voidptr ptr) {
    return;
}

void init_heap(void) {
    heap.position = 0;
    heap.top = ADDR_KERNEL_HEAP;

    heap.alloc = alloc;
    heap.free = free;
}

struct Heap *get_heap(void) {
    return &heap;
}