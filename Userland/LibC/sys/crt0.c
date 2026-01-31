#include <stdint.h>
#include <string.h>
#include <sys/system.h>

extern uint8_t __libc_bss_start[];
#define bss_start access_mutable_global_array(__libc_bss_start, uint8_t)

extern uint8_t __libc_bss_end__[];
#define bss_end access_mutable_global_array(__libc_bss_start, uint8_t)

void rom_functions_init();

void _init()
{
    rom_functions_init();

    memset(bss_start, 0, bss_end - bss_start);

    // FIXME: Call preinit array

    // FIXME: Call init array
}

void _fini()
{
    // FIXME: Call fini array
}
