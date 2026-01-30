#pragma once

#include "klib/types.h"

#define SHELL_ADDRESS                   0x210000
#define PROG_ADDRESS                    0x400000
#define ARGUMENTS_ADDRESS               0x200000

#define MAX_ARGUMENTS                   32

struct CommandLineArguments {
    int argc;
    char* argv[MAX_ARGUMENTS];
};

struct ProgramLoader {
    void (*load)(string path, u32 bytes, u32 address, struct CommandLineArguments* args);
    void (*exec)(void);
};

void init_program_loader(void);
struct ProgramLoader* get_program_loader(void);