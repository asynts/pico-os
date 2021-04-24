#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#define rom_table_code(code1, code2) ((code2) << 8 | (code1))

#define ROM_MEMCPY 0
#define ROM_MEMSET 1
#define ROM_MAX 2

// We have to phrase it like this, because otherwise the compiler could put it into
// the .bss section, resetting it back to zero since it calls abi functions before clearing
// that section.
//
// It seems that if we make this static, the compiler tries to do some sort of optimization
// where it just changes the value to be able to put it into the .bss section.  Thus this
// symbol is defined globally and marked volatile.
volatile int rom_functions_are_not_initialized = 1;

static uint32_t rom_functions[] = {
    [ROM_MEMCPY] = rom_table_code('M', 'C'),
    [ROM_MEMSET] = rom_table_code('M', 'S'),
};

static void rom_functions_init()
{
    printf("[rom_functions_init] rom_functions_are_not_initialized=%i\n", rom_functions_are_not_initialized);

    if (rom_functions_are_not_initialized == 0) {
        printf("[rom_functions_init] ROM functions already initialized...\n");
        return;
    }
    rom_functions_are_not_initialized = 0;

    printf("[rom_functions_init] rom_functions_are_not_initialized=%i (after clearing it)\n", rom_functions_are_not_initialized);

    typedef uint32_t (*fn_table_lookup)(uint16_t *table, uint32_t code);

    uint16_t *table_lookup_ptr = (uint16_t*)0x00000018;
    fn_table_lookup table_lookup = (fn_table_lookup)(uint32_t)*table_lookup_ptr;

    uint16_t *func_table_ptr = (uint16_t*)0x00000014;
    uint16_t *func_table = (uint16_t*)(uint32_t)*func_table_ptr;

    for (size_t i = 0; i < ROM_MAX; ++i) {
        uint32_t rom_function_code = rom_functions[i];
        uint32_t value = rom_functions[i] = table_lookup(func_table, rom_functions[i]);
        if (value == 0)
            printf("Could not find ROM function %u (i=%zu)\n", rom_function_code, i);
        assert(value);
    }
}

void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
    rom_functions_init();

    typedef uint8_t* (*fn_memcpy)(void*, const void*, size_t);
    ((fn_memcpy)rom_functions[ROM_MEMCPY])(dest, src, n);
}

void __aeabi_memset(void *dest, size_t n, int c)
{
    rom_functions_init();

    typedef uint8_t* (*fn_memset)(uint8_t *ptr, uint8_t c, uint32_t n);
    ((fn_memset)rom_functions[ROM_MEMSET])(dest, (uint8_t)c, n);
}

void __aeabi_memclr(void *dest, size_t n)
{
    __aeabi_memset(dest, n, 0);
}
