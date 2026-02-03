#include "mkdir.h"
#include <cli.h>
#include <stdio.h>
#include <syscall.h>
#include <string.h>

int main() {
    benix_GetCLIArgs();

    int result;

    if (argc < 2) {
        printf("Missing directory name\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], CMD_HELP_SHORT) == 0 || strcmp(argv[i], CMD_HELP_LONG) == 0) {
            printf("mkdir -- creates a new directory\n");
            printf("usage: mkdir <dirname>\n");
            printf("-h / --help                 : display this message\n");
        }

        else {
            result = syscall_dirnew(argv[i]);
            switch (result) {
                case 0:
                    return 1;;

                case -1:
                    printf("Directory not found: %s\n", argv[i]);
                    return 1;

                case -2:
                    printf("Parent path is not a directory\n");
                    return 1;

                case -3:
                    printf("Already exists: %s\n", argv[i]);
                    return 1;

                case -4:
                    printf("No more disk space available\n");
                    return 1;

                default:
                    printf("An error occured\n");
                    return 1;
            }
        }
    }

    return 0;
}