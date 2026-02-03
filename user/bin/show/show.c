#include "show.h"
#include <string.h>
#include <syscall.h>
#include <cli.h>
#include <stdio.h>

int main() {
    benix_GetCLIArgs();

    char buffer[512 * 10];
    int result;

    if (argc < 2) {
        printf("Filename required\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], CMD_HELP_SHORT) == 0 || strcmp(argv[i], CMD_HELP_LONG) == 0) {
            printf("show -- displays the content of a file in ASCII\n");
            printf("Usage: show <filename>\n");
            printf("-h / --help                 : display this message\n");
        }

        else {
            result = fread(argv[i], buffer, sizeof(buffer));
            if (result == -1) {
                printf("File not found: %s\n", argv[i]);
                return 1;
            } else if (result == -2) {
                printf("Is a directory: %s\n", argv[i]);
                return 1;
            } else {
                printf("%s", buffer);
            }
        }
    }

    return 0;
}