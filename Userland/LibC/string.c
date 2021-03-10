#include <string.h>
#include <stdlib.h>

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
    abort();
}

void* memset(void *dest, int ch, size_t count)
{
    for (size_t index = 0; index < count; ++index)
        ((char*)dest)[index] = ch;

    return dest;
}
