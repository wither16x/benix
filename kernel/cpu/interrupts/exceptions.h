/*
    exceptions.h

    This header provides structures and functions to handle CPU exceptions and get informations from
    them.
*/

#pragma once

#include "klib/types.h"

/*
    State of the CPU before calling the exception handler
*/
struct CPUState {
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 int_no;
    u32 err_code;
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 useresp;
    u32 ss;
} pt_regs_t;

// Handle CPU exceptions
__attribute__((noreturn))
void exception_handler(struct CPUState* cpu_state);