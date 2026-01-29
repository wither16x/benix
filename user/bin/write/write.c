#include "write.h"
#include <cli.h>
#include <stdio.h>
#include <string.h>

void main() {
    benix_GetCLIArgs();

    char buffer[1024];
    int result;

    if (argc < 2) {
        printf("Filename required\n");
        return;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], CMD_HELP_SHORT) == 0 || strcmp(argv[i], CMD_HELP_LONG) == 0) {
            printf("write -- writes text into a file\n");
            printf("usage: write <filename>/[options]\n");
            printf("-h / --help                 : display this message\n");
            continue;
        }

        else {
            input(buffer);
            result = fwrite(argv[i], buffer, strlen(buffer));

            if (result == -1) {
                printf("File not found: %s\n", argv[i]);
            } else if (result == -2) {
                printf("Is a directory: %s\n", argv[i]);
            } else if (result == -3) {
                printf("Failed to write in %s\n", argv[i]);
            }
        }
    }
}