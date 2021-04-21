#include <stdint.h>
#include <assert.h>

#define rom_table_code(code1, code2) ((code2) << 8 | (code1))

#define ROM_MEMCPY 0
#define ROM_MEMSET 1
#define ROM_MAX 2

static int rom_functions_initialized = 0;
static uint32_t rom_functions[] = {
    [ROM_MEMCPY] = rom_table_code('M', 'C'),
    [ROM_MEMSET] = rom_table_code('M', 'S'),
};

static void rom_functions_init()
{
    if (rom_functions_initialized)
        return;
    rom_functions_initialized = 1;

    typedef uint32_t (*fn_table_lookup)(uint16_t *table, uint32_t code);

    uint16_t *table_lookup_ptr = (uint16_t*)0x00000018;
    fn_table_lookup table_lookup = (fn_table_lookup)(uint32_t)*table_lookup_ptr;

    uint16_t *func_table_ptr = (uint16_t*)0x00000014;
    uint16_t *func_table = (uint16_t*)(uint32_t)*func_table_ptr;

    for (size_t i = 0; i < ROM_MAX; ++i) {
        uint32_t value = rom_functions[i] = table_lookup(func_table, rom_functions[i]);
        assert(value);
    }
}

void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
    rom_functions_init();

    typedef void (*fn_memcpy)(void*, const void*, size_t);
    ((fn_memcpy)rom_functions[ROM_MEMCPY])(dest, src, n);
}
