#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/abi.h>

char* strchr(const char *str, int ch)
{
    for (;;) {
        if (*str == 0)
            return NULL;

        if (*str == ch)
            return (char*)str;

        ++str;
    }
}

char* strtok_r(char *str, const char *delim, char **saveptr)
{
    if (str == NULL) {
        // Skip all leading delimiters.
        while (strchr(delim, **saveptr))
            ++(*saveptr);

        char *begin = *saveptr;

        // Find the next delimiter.
        for(;;) {
            if (**saveptr == 0) {
                if (begin == *saveptr)
                    return NULL;

                return begin;
            }

            if (strchr(delim, **saveptr)) {
                **saveptr = 0;
                ++(*saveptr);

                return begin;
            }

            ++(*saveptr);
        }

        abort();
    } else {
        *saveptr = str;
        return strtok_r(NULL, delim, saveptr);
    }
}

int strcmp(const char *lhs, const char *rhs)
{
    while(*lhs && *lhs == *rhs) {
        ++lhs;
        ++rhs;
    }

    return *lhs - *rhs;
}

void* memset(void *dest, int ch, size_t count)
{
    __aeabi_memset(dest, count, ch);
    return dest;
}

void* memcpy(void *dest, const void *src, size_t count)
{
    __aeabi_memcpy(dest, src, count);
    return dest;
}

size_t strlen(const char *str)
{
    size_t length = 0;
    while (*str++)
        ++length;

    return length;
}

char* strdup(const char *str)
{
    char *copy = malloc(strlen(str) + 1);
    return strcpy(copy, str);
}

char* strcpy(char *dest, const char *src)
{
    return memcpy(dest, src, strlen(src) + 1);
}

char* strcat(char *dest, const char *src)
{
    strcpy(dest + strlen(dest), src);
    return dest;
}
