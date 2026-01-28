#pragma once

void syscall_printchar(int c);
void syscall_printstr(char* s);
void syscall_cls(void);
int syscall_fread(char* path, char* buf, int size);
int syscall_lsdir(char* path, char* buf);
int syscall_getchar(void);
int syscall_fnew(char* path);
int syscall_fwrite(char* path, char* buf, int size);