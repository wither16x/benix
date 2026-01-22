#include "touch.h"
#include <syscall.h>
#include <cli.h>
#include <stdio.h>
#include <string.h>

void main() {
    benix_GetCLIArgs();

    if (argc < 2) {
        printf("Missing filename\n");
        return;
    }

    if (strcmp(argv[1], CMD_HELP_SHORT) == 0 || strcmp(argv[1], CMD_HELP_LONG) == 0) {
        printf("touch -- creates an empty file\n");
        printf("usage: touch <filename>\n");
        printf("-h / --help                 : display this message\n");
    } else {
        int result = syscall_fnew(argv[1]);
        switch (result) {
            case 0:
                break;

            case -1:
                printf("Path is not valid\n");
                break;

            case -2:
                printf("Parent path is not a directory\n");
                break;

            case -3:
                printf("This file already exists\n");
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