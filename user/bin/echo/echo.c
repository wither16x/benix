#include <cli.h>
#include <stdio.h>
#include <string.h>

#include "echo.h"

int main() {
    benix_GetCLIArgs();

    if (argc < 2) {
        return 1;
    }

    if (strcmp(argv[1], CMD_HELP_SHORT) == 0 || strcmp(argv[1], CMD_HELP_LONG) == 0) {
        printf("echo -- displays a string followed by a newline\n");
        printf("Usage: echo [text]\n");
        printf("NONE                        : display a newline\n");
        printf("-h / --help                 : display this message\n");
        printf("[text]                      : display the given text\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    return 0;
}