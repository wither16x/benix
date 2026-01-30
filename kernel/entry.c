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

void kmain(void) {
    // initialize kernel components
    install_video();
    install_gdt_idt();
    install_drivers();
    install_proc();
    install_tss();

    // enable interrupts
    STI

    // switch to usermode
    enter_usermode(0x210000);

    // loop on HLT to save power
    while (true) {
        HLT
    }
}