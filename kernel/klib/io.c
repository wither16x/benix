/*
    io.c

    Implementations of the functions in io.h.
*/

#include "klib/io.h"
#include "klib/bool.h"
#include "klib/string.h"
#include "drivers/console.h"
#include "drivers/fs/fat12.h"
#include "drivers/keyboard.h"
#include "types.h"

void putchar(u8 c) {
    get_driver_console()->putchar(c);
}

void print(string s) {
    get_driver_console()->print(s);
}

void puts(string s) {
    get_driver_console()->print(s);
    get_driver_console()->print("\n");
}

void vformat(string out, size_t out_sz, string fmt, va_list args) {
    size_t i = 0;
    char buffer[32];

    while (*fmt && i + 1 < out_sz) {
        if (*fmt == '%') {
            fmt++;

            switch (*fmt) {
                case 'c': {
                    char ch = (char)va_arg(args, i32);
                    out[i++] = ch;
                    break;
                }

                case 's': {
                    string str = va_arg(args, string);
                    while (*str && i + 1 < out_sz - 1)
                        out[i++] = *str++;
                    break;
                }

                case 'b': {
                    i32 val = va_arg(args, i32);
                    itoa(val, buffer, 2);
                    size_t j = 0;
                    while (buffer[j] != '\0' && i + 1 < out_sz) {
                        out[i++] = buffer[j++];
                    }
                    break;
                }

                case 'o': {
                    i32 val = va_arg(args, i32);
                    itoa(val, buffer, 8);
                    size_t j = 0;
                    while (buffer[j] != '\0' && i + 1 < out_sz) {
                        out[i++] = buffer[j++];
                    }
                    break;
                }

                case 'd': {
                    i32 val = va_arg(args, i32);
                    itoa(val, buffer, 10);
                    size_t j = 0;
                    while (buffer[j] != '\0' && i + 1 < out_sz) {
                        out[i++] = buffer[j++];
                    }
                    break;
                }

                case 'x': {
                    i32 val = va_arg(args, i32);
                    itoa(val, buffer, 16);
                    size_t j = 0;
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

void vprintf(string fmt, va_list args) {
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
                    string str = va_arg(args, string);
                    print(str);
                    break;
                }

                case 'b': {
                    i32 i = va_arg(args, i32);
                    itoa(i, buffer, 2);
                    print(buffer);
                    break;
                }

                case 'o': {
                    i32 i = va_arg(args, i32);
                    itoa(i, buffer, 8);
                    print(buffer);
                    break;
                }

                case 'd': {
                    i32 i = va_arg(args, i32);
                    itoa(i, buffer, 10);
                    print(buffer);
                    break;
                }

                case 'x': {
                    i32 i = va_arg(args, i32);
                    itoa(i, buffer, 16);
                    print(buffer);
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

void printf(string fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

char getchar() {            
    return get_driver_keyboard()->getchar();
}

void getstrend(string buf, char end) {
    u32 i = 0;
    char c = 0;
    while (c != end) {
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

void getstr(string buf) {
    getstrend(buf, '\n');
}

void itoa(i32 i, string buffer, i32 base) {
    string p = buffer;
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
        i32 digit = i % base;
        *p++ = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
        i /= base;
    }

    if (negative) {
        *p++ = '-';
    }

    *p = '\0';

    string start = buffer;
    string end = p - 1;
    while (start < end) {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
}

i32 fread(const string path, u8* buffer, u32 size) {
    return get_fsdriver_fat12()->read_file(path, buffer, size);
}

i32 lsdir(const string path, string buffer) {
    return get_fsdriver_fat12()->read_dir(path, buffer);
}

i32 findfile(string path) {
    return get_fsdriver_fat12()->lookup(path);
}

i32 fnew(string path) {
    return get_fsdriver_fat12()->create_file(path);
}