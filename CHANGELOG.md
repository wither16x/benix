# Benix changelog

## Version 0.0.3 (UNFINISHED)
- General:
    - added `memory_map.txt`
    - improved `.gitignore`
- Tools:
    - added `image [path]` subcommand to the installer
- Benlibc:
    - added `limits.h`
- Userspace:
    - added programs: `touch`, `write`, `mkdir`, `rm`
    - replaced syscall 7 (`ffind`) by `fnew`, added syscalls: 8 (`fwrite`),
      9 (`dirnew`), 10 (`frem`) fixed `syscall_fread()`
    - better error handling in user programs
    - added `LICENSE.txt`
    - reorganized files and directories
    - edited `README.txt`
- Drivers:
    - FAT12:
        - supports removing files
        - supports file and directory creation and writing in files
        - cleaned up API
        - big refactorisation
        - subdirectories support
    - VGA driver supports scrollup
- Klib:
    - fixed `__asm_outw()` (stupid error, I was using `outb` instead of `outw`)
    - added `strncpy()`, `strcpy()` and `strrchr()`
- Bootloader:
    - cleaned up `gdt.asm`
- Build:
    - only one disk image is needed (instead of two)
    - replaced build scripts by the Benix installer

## Version 0.0.2
- General:
    - updated build instructions in `README.md`
- Benlibc:
    - a lot of improvements (new headers and functions)
- Memory:
    - remade memory map
- Userspace:
    - added programs: `cash`, `ls`, `show`, `info`, `echo`
    - remade `enter_usermode()`
    - added syscall 7 (`ffind`)
    - added `osinfo.txt`
    - removed OS version from `README.txt`
- Drivers:
    - added private function `format_filename()` to the FAT12 driver
    - added public function `lookup()` to the FAT12 driver
- Klib:
    - added function `toupper()`
    - fixed `strtok()`
    - added function `findfile()`
    - fixed `getstrend()` by initializing `c` since it corrupted the memory. Backspaces are now handled correctly
- Build:
    - now using Amber instead of BASH for the scripts
    - scripts are now located in `scripts/`

## Version 0.0.1
- Updated `welcome.c`
- Fixed switch to usermode, so that syscalls do not make #UD happen anymore
- Reorganized syscalls
- `console.h` and `console.c` are now in the `drivers/` directory. `consoles/` has been removed
- The VGA cursor is reset when the screen is cleared using the console driver `clear()` function
- Added basic libc that does not respect any C standard (`benlibc/`)
- Organized makefiles

## Version 0.0.0
- Initial release