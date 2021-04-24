#include <stdint.h>
#include <string.h>

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

void _init()
{
    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    // FIXME: Call preinit array

    // FIXME: Call init array
}

void _fini()
{
    // FIXME: Call fini array
}
