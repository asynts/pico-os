#include <stdint.h>

#define rom_func_table ((uint16_t*)0x00000014)
#define rom_data_table ((uint16_t*)0x00000016)
#define rom_table_lookup_ptr ((void*(*)(uint16_t*, uint32_t))0x00000014)

void* rom_func_lookup(uint32_t code)
{
    return (*rom_table_lookup_ptr)(rom_func_table, code);
}

void* rom_data_lookup(uint32_t code)
{
    return (*rom_table_lookup_ptr)(rom_data_table, code);
}

void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
    typedef void (*fn_memcpy)(void*, const void*, size_t);
    return ( (fn_memcpy)rom_func_lookup('M' << 8 | 'C') )(dest, src, n);
}

void __aeabi_memcpy4(void *dest, const void *src, size_t n)
{
    typedef void (*fn_memcpy4)(void*, const void*, size_t);
    return ( (fn_memcpy4)rom_func_lookup('C' << 8 | '4') )(dest, src, n);
}
