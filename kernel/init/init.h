#pragma once

#include "klib/types.h"

void install_video(void);
void recover_bootinfo(u32 multiboot2);
void install_gdt_idt(void);
void install_memory(void);
void install_drivers(void);
void install_proc(void);
void install_tss(void);