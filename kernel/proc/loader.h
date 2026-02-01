#pragma once

#include "klib/types.h"

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