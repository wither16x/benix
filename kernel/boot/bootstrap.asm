; bootstrap.asm
;
; This is a small bootstrap that simply calls the kernel and could set up some things before doing that:
; - Canceling interruots
; - Setting up the kernel stack
;

[bits 32]

%define KERNEL_STACK                    0x80000

section .text
global _start
extern kmain

_start:
    cli
    mov esp, KERNEL_STACK

    call kmain

hang:
    hlt
    jmp hang