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

void* malloc(size_t size)
{
    printf("malloc(%zu)\n", size);
    printf("__heap_start__=%p\n", __heap_start__);

    // We crash when accessing heap, this appears to be random though, if I create
    // an 'x' variable, this doesnt' happen.

    // Maybe this is related to the "inconsistent use of R9" thing?
    printf("heap=%p\n", heap);

    if (heap == NULL)
        heap = __heap_start__;

    heap += size;
    return heap - size;
}

void* realloc(void *pointer, size_t size)
{
    return pointer;
}
