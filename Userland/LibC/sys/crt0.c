#include <stdint.h>
#include <string.h>

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

typedef void (*callback_t)();

extern callback_t __preinit_array_start[];
extern callback_t __preinit_array_end[];

extern callback_t __init_array_start[];
extern callback_t __init_array_end[];

extern callback_t __fini_array_start[];
extern callback_t __fini_array_end[];

static void call_callback_array(callback_t *start, callback_t *end)
{
    for (callback_t *callback = start; callback < end; callback++)
        (*callback)();
}

void _init()
{
    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    call_callback_array(__preinit_array_start, __preinit_array_end);
    call_callback_array(__init_array_start, __init_array_end);
}

void _fini()
{
    call_callback_array(__fini_array_start, __fini_array_end);
}
