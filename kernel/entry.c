/*
    main.c

    This program contains the kernel entry point (kmain). It initializes all the drivers, does some tests
    and enables interrupts. It ends by an infinite loop on the HLT instruction to save power.
*/

#include "cpu/rings.h"
#include "cpu/tss.h"
#include "init/init.h"
#include "klib/bool.h"
#include "klib/asm.h"
#include "memory.h"

struct Multiboot2_Info* pbootinfo;

void kmain(u32 mb2_tag_addr) {
    // initialize kernel components
    install_video();
    recover_bootinfo(mb2_tag_addr);
    install_gdt_idt();
    install_memory();
    install_drivers();
    install_proc();
    install_tss();

    // enable interrupts
    STI

    // switch to usermode
    enter_usermode(ADDR_SHELL);

    // loop on HLT to save power
    while (true) {
        HLT
    }
}