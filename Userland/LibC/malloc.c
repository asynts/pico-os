#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/system.h>

extern char __libc_heap_start[];
#define heap_start access_mutable_global(__libc_heap_start, char*)

extern char __libc_heap_end[];
#define heap_end access_mutable_global(__libc_heap_start, char*)

// FIXME: Implement a proper malloc.

void free(void *pointer)
{
}

static char *__libc_heap;
#define heap access_mutable_global(__libc_heap, char*)

static size_t round_to_word(size_t size)
{
    if (size % 4 != 0)
        size = size + 4 - size % 4;
    return size;
}

void* malloc(size_t size)
{
    if (heap == NULL)
        heap = heap_start;

    size = round_to_word(size);

    heap += size;

    char *pointer = heap - size;
    assert(pointer <= heap_end);
    return pointer;
}

void* realloc(void *pointer, size_t size)
{
    return pointer;
}
