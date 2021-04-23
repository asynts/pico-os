#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

typedef void (*callback_t)(void);

extern const callback_t __preinit_array_start[];
extern const callback_t __preinit_array_end[];

extern const callback_t __init_array_start[];
extern const callback_t __init_array_end[];

extern const callback_t __fini_array_start[];
extern const callback_t __fini_array_end[];

void _init()
{
    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    // FIXME: Call preinit array

    printf("[_init] calling init-array\n");
    for (const callback_t *callback = __init_array_start; callback < __init_array_end; ++callback) {
        // FIXME: There is one additionaly entry at the start?

        // FIXME: Why does the compile set the thumb bit here?
        callback = (const callback_t*)((uint32_t)callback & ~1);

        printf("[_init] callback=%p\n", callback);

        // FIXME: This needs to be relative to the readonly segment
        printf("[_init] *callback=%p\n", *callback);
    }
}

void _fini()
{
    // FIXME: Call fini array
}
