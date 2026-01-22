#include "show.h"
#include <string.h>
#include <syscall.h>
#include <cli.h>
#include <stdio.h>

void main() {
    benix_GetCLIArgs();

    char buffer[512 * 10];

    if (argc < 2) {
        printf("Filename required\n");
        return;
    }

    if (strcmp(argv[1], CMD_HELP_SHORT) == 0 || strcmp(argv[1], CMD_HELP_LONG) == 0) {
        printf("show -- displays the content of a file in ASCII\n");
        printf("Usage: show <filename>\n");
        printf("-h / --help                 : display this message\n");
        return;
    }

    int result = fread(argv[1], buffer, sizeof(buffer));
    if (result == -1) {
        printf("File not found: %s\n", argv[1]);
    } else if (result == -2) {
        printf("Is a directory: %s\n", argv[1]);
    } else {
        printf("%s", buffer);
    }
}