#include <stdint.h>
#include <string.h>

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

void rom_functions_init();

void _init()
{
    rom_functions_init();

    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    // FIXME: Call preinit array

    // FIXME: Call init array
}

void _fini()
{
    // FIXME: Call fini array
}
