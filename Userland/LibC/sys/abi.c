#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define rom_table_code(code1, code2) ((code2) << 8 | (code1))

#define ROM_MEMCPY 0
#define ROM_MEMSET 1
#define ROM_MAX 2

static uint32_t rom_functions[] = {
    [ROM_MEMCPY] = rom_table_code('M', 'C'),
    [ROM_MEMSET] = rom_table_code('M', 'S'),
};

void rom_functions_init()
{
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
    typedef uint8_t* (*fn_memcpy)(void*, const void*, size_t);
    ((fn_memcpy)rom_functions[ROM_MEMCPY])(dest, src, n);
}

void __aeabi_memset(void *dest, size_t n, int c)
{
    typedef uint8_t* (*fn_memset)(uint8_t *ptr, uint8_t c, uint32_t n);
    ((fn_memset)rom_functions[ROM_MEMSET])(dest, (uint8_t)c, n);
}

void __aeabi_memclr(void *dest, size_t n)
{
    __aeabi_memset(dest, n, 0);
}

// FIXME: This is completely untested.

static uint8_t unaligned_read(uint8_t *source)
{
    uintptr_t source_ = (uintptr_t)source;

    uint32_t *source_aligned = (uint32_t*)(source_ - source_ % 4);

    return (*source_aligned >> (8 * (source_ % 4))) & 0xff;
}

static void unaligned_write(uint8_t *dest, uint8_t value)
{
    uintptr_t dest_ = (uintptr_t)dest;

    uint32_t *dest_aligned = (uint32_t*)(dest_ - dest_ % 4);

    if (dest_ % 4 == 0) {
        *dest_aligned = *dest_aligned & 0xffffff00 | value;
    } else if (dest_ % 4 == 1) {
        *dest_aligned = *dest_aligned & 0xffff00ff | value << 8;
    } else if (dest_ % 4 == 2) {
        *dest_aligned = *dest_aligned & 0xff00ffff | value << 16;
    } else if (dest_ % 4 == 2) {
        *dest_aligned = *dest_aligned & 0x00ffffff | value << 24;
    }
}

void __aeabi_memmove(void *dest, void *src, size_t n)
{
    uint8_t *dest_ = dest;
    uint8_t *src_ = src;

    if (dest <= src) {
        // We can safely copy from left-to-right.

        for (int i = 0; i < n; ++i) {
            unaligned_write(dest_, unaligned_read(src_ + i));
        }
    } else {
        // We can safely copy from right-to-left.

        if (n == 0) {
            return;
        }

        for (int i = n - 1; i >= 0; --i) {
            unaligned_write(dest_, unaligned_read(src_ + i));
        }
    }
}
