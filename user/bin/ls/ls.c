#include <cli.h>
#include <stdio.h>
#include <syscall.h>
#include <string.h>

#include "ls.h"

void main() {
    benix_GetCLIArgs();

    char buffer[224 * 11];
    int result;

    if (argc < 2) {
        syscall_lsdir("/", buffer);
        return;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], CMD_HELP_SHORT) == 0 || strcmp(argv[i], CMD_HELP_LONG) == 0) {
            printf("ls -- displays the name of the files and folders in a directory\n");
            printf("Usage: ls [dir]\n");
            printf("NONE                        : display the content of /\n");
            printf("-h / --help                 : display this message\n");
            printf("[dir]                       : display the content of the directory\n");
        }

        else {
            result = syscall_lsdir(argv[i], buffer);
            if (result == -1) {
                printf("Directory not found: %s\n", argv[i]);
            } else if (result == -2) {
                printf("Not a directory: %s\n", argv[i]);
            } else {
                printf("%s", buffer);
            }
        }
    }
}