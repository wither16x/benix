#include "mm/vmm.h"
#include "klib/asm.h"
#include "mm/pmm.h"
#include "klib/null.h"
#include "klib/memory.h"
#include "mm/paging.h"

static struct VMM vmm;
static struct PageDirectory* pd;

static struct PageTable* alloc_table(void) {
    struct PageTable* pt;
    u64 frame = get_pmm()->alloc();
    if (frame == -1) {
        return NULL;
    }

    pt = (struct PageTable*)(frame * PAGE_SIZE);
    memset(pt, 0, sizeof(struct PageTable));

    return pt;
}

static void map(u32 virt, u32 phys, u32 flags) {
    u32 idx_pd = virt >> 22;
    u32 idx_pt = (virt >> 12) & 0x3ff;
    u32 pde_flags = PAGE_PRESENT | PAGE_RW;
    struct PageTable* pt;

    if (!(pd->entries[idx_pd] & 1)) {
        pt = alloc_table();
        if (!pt) {
            return;
        }

        if (flags & PAGE_USER) {
            pde_flags |= PAGE_USER;
        }

        pd->entries[idx_pd] = ((u32)pt & PAGE_ADDRESS_MASK) | pde_flags;
    } else {
        pt = (struct PageTable*)(pd->entries[idx_pd] & PAGE_ADDRESS_MASK);
        if (flags & PAGE_USER) {
            pd->entries[idx_pd] |= PAGE_USER;
        }
    }

    pt->entries[idx_pt] = (phys & 0xfffff000) | flags;

    if (is_paging_enabled()) {
        INVLPG((voidptr)virt)
    }
}

void init_vmm(void) {
    pd = (struct PageDirectory*)alloc_table();
    memset(pd, 0, sizeof(struct PageDirectory));

    vmm.alloc_table = alloc_table;
    vmm.map = map;
}

struct VMM* get_vmm(void) {
    return &vmm;
}

struct PageDirectory* get_kernel_page_table(void) {
    return pd;
}