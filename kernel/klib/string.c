/*
    string.c

    Implementations of the functions in string.h.
*/

#include "klib/string.h"
#include "klib/null.h"

u32 strlen(string s) {
    u32 i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

string strchr(string s, i32 c) {
    while (*s) {
        if (*s == (char)c) {
            return (string)s;
        }
        s++;
    }

    return NULL;
}

string strtok(string s, const string delim) {
    static string next = NULL;

    if (s != NULL)
        next = s;
    else if (next == NULL)
        return NULL;

    while (*next && strchr(delim, *next))
        next++;

    if (*next == '\0') {
        next = NULL;
        return NULL;
    }

    string start = next;

    while (*next && !strchr(delim, *next))
        next++;

    if (*next) {
        *next = '\0';
        next++;
    } else {
        next = NULL;
    }

    return start;
}

i32 strcmp(string s1, string s2) {
    while ((*s1 != '\0' && *s2 != '\0') && *s1 == *s2) {
        s1++;
        s2++;
    }

    return (*s1 == *s2) ? 0 : (*s1 > *s2) ? 1 : -1;
}

i32 toupper(i32 c) {
    if (c >= 'a' && c <= 'z') {
        return  c - ('a' - 'A');
    }
    return c;
}

void strcpy(string dest, string src) {
    while ((*dest++ = *src++));
}

void strncpy(string dest, string src, i32 size) {
    i32 i = 0;
    while (i++ != size && (*dest++ = *src++));
}