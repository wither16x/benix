/*
    string.h

    This header provides functions to work with strings.
*/

#pragma once

#include "klib/types.h"

// Get the length of a null-terminated string
u32 strlen(string s);
// Find the first occurence of a character in a string
string strchr(string s, i32 c);
// Tokenize a string
string strtok(string s, const string delim);
// Compare two strings
i32 strcmp(string s1, string s2);
// Convert a character to uppercase
i32 toupper(i32 c);
// Copy a string
void strcpy(string dest, string src);
// Copy a given amount of characters of a string
void strncpy(string dest, string src, i32 size);