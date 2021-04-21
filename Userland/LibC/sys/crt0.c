#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

typedef void (*callback_t)(void);

extern callback_t __preinit_array_start[];
extern callback_t __preinit_array_end[];

extern callback_t __init_array_start[];
extern callback_t __init_array_end[];

extern callback_t __fini_array_start[];
extern callback_t __fini_array_end[];

void _init()
{
    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    printf("[_init] calling preinit-array\n");
    for (callback_t *callback = __preinit_array_start; callback < __preinit_array_end; ++callback) {
        printf("[_init] callback=%p *callback=%p\n", callback, *callback);
        (*callback)();
    }

    printf("[_init] calling init-array\n");
    for (callback_t *callback = __init_array_start; callback < __init_array_end; ++callback) {
        callback = (callback_t*)((uint32_t)callback & ~1);

        printf("[_init] callback=%p\n", callback);
        printf("[_init] *callback=%p\n", *callback);
        (*callback)();
    }
}

void _fini()
{
    // call_callback_array((callback_t *const)__fini_array_start, (callback_t *const)__fini_array_end);
}
