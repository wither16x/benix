; stage2.asm
;
; This is the second stage of the bootloader. Here, we can do more things, because we are not limited
; by the size of the boot sector (in fact, we are limited by the number of sectors loaded by the boot
; sector).
; We do the following things in order:
; 1. Initialize the segments at 0x7e0:0
; 2. Set the stack at 0x8000 + 0xf000
; 3. Save the boot device byte (saved in dl by the boot sector right before the jump)
; 4. Load the kernel at 0x1000:0
; 5. Load the GDT (Global Descriptor Table) defined in gdt.asm
; 6. Switch to protected mode
; 7. Initialize protected mode
; 8. Copy the kernel from 0x1000:0 at 0x100000
; 9. Jump to the kernel
;

[bits 16]
[org 0x7e00]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    cli                     ; better to disable interrupts when changing SS
    mov ax, 0x8000
    mov ss, ax
    mov sp, 0xf000
    sti

    mov [driveno], dl       ; driveno has been saved into dl before we jumped to start

    push es
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 50
    mov ch, 0x00
    mov cl, 0xb                ; load kernel at sector 11, depends of the size of this program
    mov dh, 0x00
    mov dl, [driveno]
    int 0x13
    jc reboot
    pop es

    cli                         ; disable interrupts
    lgdt [gdtptr]               ; load the GDT
    ; set PE bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:pmode_init

; the two functions below are "documented" in bootsect.asm
reboot:
    mov si, reboot_msg
    call print
    xor ah, ah
    int 0x16

    xor ah, ah
    int 0x19

print:
    push ax
    push bx
    push si
    
    mov ah, 0x0e
.next_char:
    lodsb
    cmp al, 0
    jz .done
    int 0x10
    jmp .next_char
.done:
    pop si
    pop bx
    pop ax

    ret

%include "bootloader/benix/gdt.asm"

driveno:                    db      0
reboot_msg:                 db      "Disk error! Press a key to reboot...", 0

[bits 32]
pmode_init:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x180000

    ; copy the kernel at 0x100000
    mov esi, 0x1000 * 16        ; physical address
    mov edi, 0x100000           ; load address
    mov ecx, 50 * 512           ; size
    cld
    rep movsb

    jmp 0x08:0x100000           ; jump to the kernel

hang:
    hlt
    jmp hang

; fill the rest of the sector with zeroes
times 512 * 10 - ($ - $$) db 0