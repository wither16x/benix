/*
    loader.c

    Implementations of the functions in loader.h.
*/

#include "proc/loader.h"
#include "klib/io.h"
#include "klib/asm.h"
#include "klib/memory.h"
#include "memory.h"

static struct ProgramLoader loader;

static void load(string path, u32 bytes, u32 address, struct CommandLineArguments* args) {
    memmove((voidptr)ADDR_ARGC_ARGV, args, sizeof(struct CommandLineArguments));
    fread(path, (u8*)address, bytes);
}

static void exec(void) {
    void (*func)(void) = (void (*)())ADDR_SHELL;
    func();
}

void init_program_loader(void) {
    loader.load = load;
    loader.exec = exec;
}

struct ProgramLoader* get_program_loader() {
    return &loader;
}