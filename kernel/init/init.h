#pragma once

#include "klib/types.h"

void recover_bootinfo(u32 multiboot2);
void install_video(void);
void install_gdt_idt(void);
void install_drivers(void);
void install_proc(void);
void install_tss(void);