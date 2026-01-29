#include "cpu/interrupts/syscalls.h"
#include "klib/io.h"
#include "drivers/console.h"

/*
    1: print character (printchar)
    2: print string (printstr)
    3: clear screen (cls)
    4: read file (fread)
    5: read directory content (lsdir)
    6: read character from keyboard (readchar)
    7: create file (fnew)
    8: write file (fwrite)
    9: create dir (dirnew)
    10: remove file (frem)
    11: remove directory (dirrem)

    Bad syscall: EAX = -1
*/
void syscall_handler(struct SyscallRegisters* r) {
    switch (r->eax) {
        case 1:
            putchar((char)r->ebx);
            break;

        case 2:
            print((string)r->ebx);
            break;

        case 3:
            get_driver_console()->clear();
            break;

        case 4:
            r->eax = fread((string)r->ebx, (u8*)r->ecx, r->edx);
            break;

        case 5:
            r->eax = lsdir((string)r->ebx, (string)r->ecx);
            break;

        case 6:
            r->eax = getchar();
            break;

        case 7:
            r->eax = fnew((string)r->ebx);
            break;

        case 8:
            r->eax = fwrite((string)r->ebx, (u8*)r->ecx, r->edx);
            break;

        case 9:
            r->eax = dirnew((string)r->ebx);
            break;

        case 10:
            r->eax = frem((string)r->ebx);
            break;

        case 11:
            r->eax = dirrem((string)r->ebx);
            break;

        default:
            r->eax = (u32)-1;
            break;
    }
}