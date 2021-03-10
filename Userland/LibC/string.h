#pragma once

#include <stddef.h>

char* strtok_r(char *str, const char *delim, char **saveptr);
int strcmp(const char *lhs, const char *rhs);
void* memset(void *dest, int ch, size_t count);
char* strchr(const char *str, int ch);
size_t strlen(const char *str);
