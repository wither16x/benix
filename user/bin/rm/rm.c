#include "rm.h"
#include <cli.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>

void main() {
    benix_GetCLIArgs();

    int result;

    if (argc < 2) {
        printf("Filename required\n");
        return;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], CMD_HELP_SHORT) == 0 || strcmp(argv[i], CMD_HELP_LONG) == 0) {
            printf("rm -- removes a file\n");
            printf("usage: rm <filename>\n");
            printf("-h / --help                 : display this message\n");
        }

        else {
            result = syscall_frem(argv[i]);
            if (result == -1) {
                printf("File not found: %s\n", argv[i]);
            } else if (result == -2) {
                printf("Is a directory: %s\n", argv[i]);
            }
        }
    }
}