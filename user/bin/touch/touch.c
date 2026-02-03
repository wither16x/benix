#include "touch.h"
#include <syscall.h>
#include <cli.h>
#include <stdio.h>
#include <string.h>

int main() {
    benix_GetCLIArgs();

    int result;

    if (argc < 2) {
        printf("Missing filename\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], CMD_HELP_SHORT) == 0 || strcmp(argv[i], CMD_HELP_LONG) == 0) {
            printf("touch -- creates an empty file\n");
            printf("usage: touch <filename>\n");
            printf("-h / --help                 : display this message\n");
        }

        else {
            result = syscall_fnew(argv[i]);
            switch (result) {
                case 0:
                    return 0;

                case -1:
                    printf("Path is not valid\n");
                    return 1;

                case -2:
                    printf("Parent path is not a directory\n");
                    return 1;

                case -3:
                    printf("This file already exists\n");
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