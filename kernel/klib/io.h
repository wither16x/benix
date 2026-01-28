/*
    io.h

    This header provides I/O functions, with the screen or the disk.
*/

#pragma once

#include "klib/types.h"
#include "klib/args.h"

// display a character
void putchar(u8 c);
// display a null-terminated string
void print(string s);
// display a null-terminated string and a newline
void puts(string s);

// format a buffer
void vformat(string out, size_t out_sz, string fmt, va_list args);
// base for printf()
void vprintf(string fmt, va_list args);
// display a formatted string
void printf(string fmt, ...);

// read a character from the keyboard
char getchar();
// read a string from the keyboard until a given character is encountered
void getstrend(string buf, char end);
// read a string from the keyboard until \n is encountered
void getstr(string buf);

// convert an integer value to a null-terminated string
void itoa(i32 i, string buffer, i32 base);

// read bytes from a file and store its content in a buffer
i32 fread(const string path, u8* buffer, u32 size);
// write bytes from a buffer into a file
i32 fwrite(const string path, const u8* buffer, u32 size);
// read the entries of a directory and store them in a buffer
// the entries are separated by newlines
i32 lsdir(const string path, string buffer);
// return a pointer to an entry in the root directory or NULL if the file does not exist
i32 findfile(string path);
// create file
i32 fnew(string path);