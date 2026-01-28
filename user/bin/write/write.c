#include "write.h"
#include <cli.h>
#include <stdio.h>
#include <string.h>

void main() {
    benix_GetCLIArgs();
    char buffer[1024];

    if (strcmp(argv[1], CMD_HELP_SHORT) == 0 || strcmp(argv[1], CMD_HELP_LONG) == 0) {
        printf("write -- writes text into a file\n");
        printf("usage: write <filename> <text>\n");
        printf("-h / --help                 : display this message\n");
        return;
    }

    if (argc < 2) {
        printf("Filename required\n");
        return;
    }

    input(buffer);

    fwrite(argv[1], buffer, sizeof(buffer));
}