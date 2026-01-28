#pragma once

#include "stdargs.h"

void putchar(int c);
void puts(const char* s);
void vformat(char* out, int out_sz, char* fmt, va_list args);
void vprintf(const char* fmt, va_list args);
void vsprintf(char* buf, char* fmt, va_list args);
void printf(const char* fmt, ...);
void sprintf(char* buf, char* fmt, ...);
void snprintf(char* buf, int len, char* fmt, ...);
int getchar();
void input(char* buf);
int fread(const char* path, char* buf, int size);
int fwrite(const char* path, const char* buf, int size);