#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

extern char __heap_start__[];
extern char __heap_end__[];

// FIXME: Implement a proper malloc.

void free(void *pointer)
{
}

static char *heap;

static size_t round_to_word(size_t size)
{
    if (size % 4 != 0)
        size = size + 4 - size % 4;
    return size;
}

void* malloc(size_t size)
{
    if (heap == NULL)
        heap = __heap_start__;

    size = round_to_word(size);

    heap += size;
    return heap - size;
}

void* realloc(void *pointer, size_t size)
{
    return pointer;
}
