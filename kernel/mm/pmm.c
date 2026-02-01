#include "mm/pmm.h"
#include "klib/types.h"
#include "klib/memory.h"

static struct PMM pmm;

static inline void set_bit(u64 bit) {
    pmm.bitmap[BYTE(bit)] |= MASK(bit);
}

static inline void clear_bit(u64 bit) {
    pmm.bitmap[BYTE(bit)] &= ~MASK(bit);
}

static inline void toggle_bit(u64 bit) {
    pmm.bitmap[BYTE(bit)] ^= MASK(bit);
}

static inline bool test_bit(u64 bit) {
    return pmm.bitmap[BYTE(bit)] & MASK(bit);
}

static u64 alloc(void) {
    for (u64 i = 0; i < pmm.total_frames; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            return i;
        }
    }
    return (u64)-1;
}

static void free(u64 frame) {
    if (frame < pmm.total_frames) {
        clear_bit(frame);
    }
}

void init_pmm(u64 total_memory, u8* bitmap) {  
    pmm.bitmap = bitmap;
    pmm.total_frames = total_memory / FRAME_SIZE;
    pmm.alloc = alloc;
    pmm.free = free;

    memset(pmm.bitmap, 0, BYTE(pmm.total_frames));
}

struct PMM* get_pmm(void) {
    return &pmm;
}