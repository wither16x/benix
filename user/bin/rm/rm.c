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
            printf("rm -- removes a file or a directory\n");
            printf("usage: rm <filename>\n");
            printf("-h / --help                 : display this message\n");
            printf("-d / --directory            : remove an empty directory\n");
        }

        else if (strcmp(argv[i], CMD_DIR_SHORT) == 0 || strcmp(argv[i], CMD_DIR_LONG) == 0) {
            if (!argv[i + 1]) {
                printf("Directory not specified\n");
                break;
            }
            
            result = syscall_dirrem(argv[i + 1]);
            if (result == -1) {
                printf("Cannot remove /\n");
                break;
            } else if (result == -2) {
                printf("Directory not found: %s\n", argv[i + 1]);
                break;
            } else if (result == -3) {
                printf("Is not a directory: %s\n", argv[i + 1]);
                break;
            } else if (result == -4) {
                printf("Directory not empty: %s\n", argv[i + 1]);
                break;
            }
            break;
        }

        else {
            result = syscall_frem(argv[i]);
            if (result == -1) {
                printf("File not found: %s\n", argv[i]);
                break;
            } else if (result == -2) {
                printf("Is a directory: %s\n", argv[i]);
                break;
            }
            break;
        }
    }
}