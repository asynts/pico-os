#include <malloc.h>
#include <stdlib.h>
#include <assert.h>

extern char __heap_start__[];
extern char __heap_end__[];

// FIXME: Implement a proper malloc.

void free(void *pointer)
{
}

static char *heap;

void* malloc(size_t size)
{
    if (heap == NULL)
        heap = __heap_start__;

    heap += size;
    return heap - size;
}

void* realloc(void *pointer, size_t size)
{
    return pointer;
}
