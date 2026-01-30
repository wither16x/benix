#include "cpu/tss.h"
#include "cpu/gdt.h"

static struct TSS tss;

void init_tss(void) {
    tss.debug_flag = 0;
    tss.io_map = 0;
    tss.esp0 = 0x180000;
    tss.ss0 = 0x10;

    get_gdt()->set_entry(5, (u32)&tss, sizeof(struct TSS) - 1, 0x89, 0);
    gdt_flush(&get_gdt()->pointer);

    tss_flush();
}

struct TSS* get_tss(void) {
    return &tss;
}