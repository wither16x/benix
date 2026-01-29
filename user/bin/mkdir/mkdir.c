#include "mkdir.h"
#include <cli.h>
#include <stdio.h>
#include <syscall.h>
#include <string.h>

void main() {
    benix_GetCLIArgs();

    int result;

    if (argc < 2) {
        printf("Missing directory name\n");
        return;
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
                    break;

                case -1:
                    printf("Directory not found: %s\n", argv[i]);
                    break;

                case -2:
                    printf("Parent path is not a directory\n");
                    break;

                case -3:
                    printf("Already exists: %s\n", argv[i]);
                    break;

                case -4:
                    printf("No more disk space available\n");
                    break;

                default:
                    printf("An error occured\n");
                    break;
            }
        }
    }
}