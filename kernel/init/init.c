#include "init/init.h"
#include "boot/bootinfo.h"
#include "boot/multiboot2.h"
#include "drivers/vga.h"
#include "drivers/console.h"
#include "klib/io.h"
#include "klib/logging.h"
#include "cpu/gdt.h"
#include "cpu/interrupts/idt.h"
#include "drivers/pic.h"
#include "drivers/ata.h"
#include "drivers/ps2.h"
#include "drivers/keyboard.h"
#include "drivers/fs/fat12.h"
#include "memory.h"
#include "proc/loader.h"
#include "cpu/tss.h"
#include "klib/null.h"

void install_video(void) {
    init_driver_vga();
    init_driver_console(get_driver_vga());
    info("initialized VGA driver");
    info("initialized VGA console");
}

void recover_bootinfo(u32 multiboot2) {
    init_bootinfo();
    info("initialized boot informations");

    multiboot2_parse(multiboot2, get_bootinfo());
    info("parsed Multiboot 2 header");

    debug("total memory: %d bytes", get_bootinfo()->total_memory);
}

void install_gdt_idt(void) {
    init_gdt();
    info("initialized and loaded new GDT");

    init_idt();
    info("initialized and loaded IDT");
}

void install_drivers(void) {
    // initialize the PIC driver, remap the PIC and make sure to mask all the IRQs
    init_driver_pic();
    info("initialized PIC driver");
    get_driver_pic()->remap(0x20, 0x28);
    info("remapped PIC at 0x20, 0x28");
    get_driver_pic()->disable();
    info("disabled all IRQs");

    // initialize the ATA driver, clear the ATA IRQ (14) mask and reset the ATA controller
    init_driver_ata();
    info("initialized ATA driver");
    get_driver_pic()->clear_irq_mask(14);
    info("cleared ATA IRQ (14)");
    get_driver_ata()->reset();
    info("reset disk");

    // initialize the PS/2 driver and clear the PS/2 IRQ (1) mask
    init_driver_ps2();
    info("initialized PS/2 driver");
    get_driver_pic()->clear_irq_mask(1);
    info("cleared PS/2 keyboard IRQ (1)");

    // initialize the keyboard driver
    init_driver_keyboard();
    info("initialized keyboard driver");

    // initialize the FAT12 driver
    init_fsdriver_fat12();
    info("initialized FAT12 filesystem driver");
}

void install_proc(void) {
    init_program_loader();
    if (!findfile("bin/cash")) {
        error("bin/cash not found");
        return;
    } else {
        get_program_loader()->load("bin/cash", 512 * 10, ADDR_SHELL, NULL);
        info("loaded user program");
    }
}

void install_tss(void) {
    init_tss();
    info("initialized TSS");
}