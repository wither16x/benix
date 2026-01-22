#include <cli.h>
#include <stdio.h>
#include <syscall.h>
#include <string.h>

#include "ls.h"

void main() {
    benix_GetCLIArgs();

    char buffer[224 * 11];

    if (argc < 2) {
        syscall_lsdir("/", buffer);
    } else if (strcmp(argv[1], CMD_HELP_SHORT) == 0 || strcmp(argv[1], CMD_HELP_LONG) == 0) {
        printf("ls -- displays the name of the files and folders in a directory\n");
        printf("Usage: ls [dir]\n");
        printf("NONE                        : display the content of /\n");
        printf("-h / --help                 : display this message\n");
        printf("[dir]                       : display the content of the directory\n");
        return;
    } else {
        int result = syscall_lsdir(argv[1], buffer);
        if (result == -1) {
            printf("Directory not found: %s\n", argv[1]);
        } else if (result == -2) {
            printf("Not a directory: %s\n", argv[1]);
        } else {
            printf("%s", buffer);
        }
    }
}