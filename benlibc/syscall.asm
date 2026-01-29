[bits 32]

section .text
global syscall_printchar, syscall_printstr, syscall_cls, syscall_fread, syscall_lsdir
global syscall_getchar, syscall_fnew, syscall_fwrite, syscall_dirnew, syscall_frem

syscall_printchar:
    mov eax, 1
    mov ebx, [esp + 4]
    int 0x80
    ret

syscall_printstr:
    mov eax, 2
    mov ebx, [esp + 4]
    int 0x80
    ret

syscall_cls:
    mov eax, 3
    int 0x80
    ret

syscall_fread:
    mov eax, 4
    mov ebx, [esp + 4]
    mov ecx, [esp + 8]
    mov edx, [esp + 12]
    int 0x80
    ret

syscall_lsdir:
    mov eax, 5
    mov ebx, [esp + 4]
    mov ecx, [esp + 8]
    int 0x80
    ret

syscall_getchar:
    mov eax, 6
    int 0x80
    ret

syscall_fnew:
    mov eax, 7
    mov ebx, [esp + 4]
    int 0x80
    ret

syscall_fwrite:
    mov eax, 8
    mov ebx, [esp + 4]
    mov ecx, [esp + 8]
    mov edx, [esp + 12]
    int 0x80
    ret

syscall_dirnew:
    mov eax, 9
    mov ebx, [esp + 4]
    int 0x80
    ret

syscall_frem:
    mov eax, 10
    mov ebx, [esp + 4]
    int 0x80
    ret