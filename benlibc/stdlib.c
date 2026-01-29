#include "stdlib.h"
#include "stdbool.h"

void itoa(int i, char* buffer, int base) {
    char* p = buffer;
    bool negative = false;

    if (i == 0) {
        *p++ = '0';
        *p = '\0';
        return;
    }

    if (i < 0 && base == 10) {
        negative = true;
        i = -i;
    }

    while (i) {
        int digit = i % base;
        *p++ = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
        i /= base;
    }

    if (negative) {
        *p++ = '-';
    }

    *p = '\0';

    char* start = buffer;
    char* end = p - 1;
    while (start < end) {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
}

int atoi(char *s) {
    int length = 0;
    int negative_count = 0;
    int count = 0;
    int number = 0;

    while (s[length] != '\0') {
        length++;
    }

    while (count < length) {
        if (s[count] == '-') {
            negative_count++;
        }

        if (s[count] >= 48 && s[count] <= 57) {
            for (; s[count] >= 48 && s[count] <= 57; count++) {
                number = 10 * number - (s[count] - 48);
            }
            break;
        }
        count++;
    }

    if (negative_count % 2 != 0) {
        return number;
    } else {
        return -number;
    }
}