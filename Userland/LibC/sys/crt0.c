#include <stdint.h>
#include <string.h>

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

void rom_functions_init();

extern void (*__preinit_array_start[])(void);
extern void (*__preinit_array_end[])(void);
extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);
extern void (*__fini_array_start[])(void);
extern void (*__fini_array_end[])(void);

void _init()
{
    rom_functions_init();

    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    for (void (**func)(void) = __preinit_array_start; func < __preinit_array_end; ++func)
        (*func)();

    for (void (**func)(void) = __init_array_start; func < __init_array_end; ++func)
        (*func)();
}

void _fini()
{
    for (void (**func)(void) = __fini_array_start; func < __fini_array_end; ++func)
        (*func)();
}
