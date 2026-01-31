; bootsect.asm
;
; This program is very minimalist, because at this stage, our goal is not to load the kernel but to exit the
; boot sector, 512 bytes (size of 1 MBR sector) is not enough for what could come next.
;
; NOTE: this boot sector is designed for FAT12 only!
;
; We do the following things in order:
; 1. Initialize the segments at 0x7c0:0
; 2. Set the stack at 0x8000 + 0xf000
; 3. Save the boot device byte into driveno, since dl could be modified
; 4. Clear the screen
; 5. Load the second stage at 0x7e0:0
; 6. Jump to the second stage
;

[bits 16]
[org 0x7c00]

jmp short start
nop

OEMLabel:                   db      "BENXBOOT"
BytesPerSector:             dw      512
SectorsPerCluster:          db      1
ReservedSectors:	        dw      100
NumberOfFats:       		db      2
RootDirEntries:             dw      224
LogicalSectors:     		dw      2880
MediumByte:                 db      0x0f0
SectorsPerFat:        		dw      9
SectorsPerTrack:    		dw      18
Sides:			            dw      2
HiddenSectors:		        dd      0
LargeSectors:		        dd      0
DriveNo:			        dw      0
Signature:		            db      41                  ; floppy
VolumeID:                   dd      0
VolumeLabel:		        db      "BENIX      "
FileSystem:		            db      "FAT12   "

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    cli                     ; better to disable interrupts when changing SS
    mov ax, 0x8000
    mov ss, ax
    mov sp, 0xf000
    sti

    mov [DriveNo], dl

    xor ah, ah
    mov al, 0x03
    int 0x10

    push es
    mov ax, 0x7e0
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 10
    mov ch, 0x00
    mov cl, 0x02            ; STAGE2 MUST BE AT SECTOR 2
    mov dh, 0x00
    mov dl, [DriveNo]
    int 0x13
    jc reboot
    pop es

    mov dl, [DriveNo]       ; set DriveNo in dl so that stage2 can recover it

    jmp 0x7e0:0

hang:                   ; should not be reached
    hlt
    jmp hang

reboot:
    mov si, reboot_msg
    call print
    xor ah, ah
    int 0x16            ; wait for a key, we can at least have indications

    xor ah, ah
    int 0x19

; SI contains the string
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

reboot_msg:                 db      "Disk error! Press a key to reboot...", 0

; fill the sector with zeroes
times 510 - ($ - $$) db 0
dw 0xaa55                   ; MBR signature