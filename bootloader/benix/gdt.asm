; gdt.asm
;
; This is a GDT to switch to protected mode.
;

[bits 16]

gdt:
    dq 0
gdt_cs:
    dw 0xffff
    dw 0
    db 0
    db 10011011b
    db 11011111b
    db 0
gdt_ds:
    dw 0xffff
    dw 0
    db 0
    db 10010011b
    db 11011111b
    db 0
gdtend:

gdtptr:
    dw gdtend - gdt - 1
    dd gdt