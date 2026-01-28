#include "stdio.h"
#include "syscall.h"
#include "stdlib.h"
#include "limits.h"
#include <stdio.h>

void putchar(int c) {
    if (c) {
        syscall_printchar(c);
    }
}

void puts(const char* s) {
    syscall_printstr((char*)s);
    syscall_printchar('\n');
}

void vformat(char* out, int out_sz, char* fmt, va_list args) {
    int i = 0;
    char buffer[32];

    while (*fmt && i + 1 < out_sz) {
        if (*fmt == '%') {
            fmt++;

            switch (*fmt) {
                case 'c': {
                    char ch = (char)va_arg(args, int);
                    out[i++] = ch;
                    break;
                }

                case 's': {
                    char* str = va_arg(args, char*);
                    while (*str && i + 1 < out_sz)
                        out[i++] = *str++;
                    break;
                }

                case 'b': {
                    int val = va_arg(args, int);
                    itoa(val, buffer, 2);
                    int j = 0;
                    while (buffer[j] != '\0' && i + 1 < out_sz) {
                        out[i++] = buffer[j++];
                    }
                    break;
                }

                case 'o': {
                    int val = va_arg(args, int);
                    itoa(val, buffer, 8);
                    int j = 0;
                    while (buffer[j] != '\0' && i + 1 < out_sz) {
                        out[i++] = buffer[j++];
                    }
                    break;
                }

                case 'd': {
                    int val = va_arg(args, int);
                    itoa(val, buffer, 10);
                    int j = 0;
                    while (buffer[j] != '\0' && i + 1 < out_sz) {
                        out[i++] = buffer[j++];
                    }
                    break;
                }

                case 'x': {
                    int val = va_arg(args, int);
                    itoa(val, buffer, 16);
                    int j = 0;
                    while (buffer[j] != '\0' && i + 1 < out_sz) {
                        out[i++] = buffer[j++];
                    }
                    break;
                }

                case '%':
                    out[i++] = '%';
                    break;

                default:
                    break;
            }
        } else {
            out[i++] = *fmt;
        }

        fmt++;
    }

    out[i] = '\0';
}

void vprintf(const char* fmt, va_list args) {
    char buffer[32];

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;

            switch (*fmt) {
                case 'c': {
                    char ch = (char)va_arg(args, int);
                    putchar(ch);
                    break;
                }

                case 's': {
                    const char* str = va_arg(args, const char*);
                    syscall_printstr((char*)str);
                    break;
                }

                case 'b': {
                    int i = va_arg(args, int);
                    itoa(i, buffer, 2);
                    syscall_printstr(buffer);
                    break;
                }

                case 'o': {
                    int i = va_arg(args, int);
                    itoa(i, buffer, 8);
                    syscall_printstr(buffer);
                    break;
                }

                case 'd': {
                    int i = va_arg(args, int);
                    itoa(i, buffer, 10);
                    syscall_printstr(buffer);
                    break;
                }

                case 'x': {
                    int i = va_arg(args, int);
                    itoa(i, buffer, 16);
                    syscall_printstr(buffer);
                    break;
                }
                
                case '%':
                    putchar('%');
                    break;

                default:
                    break;
            }
        } else {
            putchar(*fmt);
        }

        fmt++;
    }
}

void vsprintf(char *buf, char *fmt, va_list args) {
    vformat(buf, INT_MAX, fmt, args);
}

void sprintf(char *buf, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
}

void snprintf(char *buf, int len, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vformat(buf, len, fmt, args);
    va_end(args);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

int getchar() {
    return syscall_getchar();
}

void input(char *buf) {
    int i = 0;
    char c = 0;
    while (c != '\n') {
        c = getchar();
        if (c) {
            if (c == '\b') {
                if (i == 0) {
                    continue;
                }
                i--;
                putchar(c);
                putchar(' ');
                putchar('\b');
                continue;
            }
            putchar(c);
            buf[i++] = c;
        }
    }
    buf[i - 1] = '\0';
}

int fread(const char* path, char *buf, int size) {
    return syscall_fread((char*)path, buf, size);
}

int fwrite(const char* path, const char* buffer, int size) {
    return syscall_fwrite((char*)path, (char*)buffer, size);
}