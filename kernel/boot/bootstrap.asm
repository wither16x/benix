; bootstrap.asm
;
; This is a small bootstrap that simply calls the kernel and could set up some things before doing that:
; - Canceling interruots
; - Setting up the kernel stack
; - Saving the multiboot tag
;

[bits 32]

%define KERNEL_STACK                    0x180000

section .multiboot2
align 8
multiboot2_header:
    dd 0xe85250d6                           ; magic
    dd 0                                    ; architecture = i386
    dd header_end - multiboot2_header
    dd -(0xe85250d6 + 0 + (header_end - multiboot2_header))

; end tag
align 8
    dw 0                                    ; type = END
    dw 0                                    ; flags
    dd 8                                    ; size
header_end:

section .text
global _start
extern kmain

_start:
    cli
    mov esp, KERNEL_STACK

    push ebx
    call kmain

hang:
    hlt
    jmp hang