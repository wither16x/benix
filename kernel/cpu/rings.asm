[bits 32]

%define USER_STACK                  0x2000000
%define USER_CS                     0x1b
%define USER_DS                     0x23

section .text
global enter_usermode

enter_usermode:
    cli

    mov ebx, [esp + 4]

    mov ax, USER_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push USER_DS
    push USER_STACK
    pushf
    pop eax
    or eax, 0x200
    push eax
    push USER_CS
    push ebx

    iret