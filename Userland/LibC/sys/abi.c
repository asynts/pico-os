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

static uint8_t unaligned_read_u8(uintptr_t source)
{
    // Address of the word in which this byte is contained.
    uint32_t *source_aligned = (uint32_t*)(source - source % 4);

    // Read the entire word.
    uint32_t value = *source_aligned;

    // This is a little endian system.
    // If we think of the decimal representation, the first bytes we read are on the right.
    // We shift to the right by eight bits for each byte we want to skip.
    value = value >> (8 * (source % 4));

    // The value is in the lowest byte, return that.
    return value & 0xff;
}

static void unaligned_write_u8(uintptr_t destination, uint8_t new_value)
{
    // Address of the word in which this byte is contained.
    uint32_t *destination_aligned = (uint32_t*)(destination - destination % 4);

    // Read the entire word.
    uint32_t value = *destination_aligned;

    // Modify the byte we want to change.
    if (destination % 4 == 0) {
        value = value & 0xffffff00 | ((uint32_t)new_value <<  0);
    } else if (destination % 4 == 1) {
        value = value & 0xffff00ff | ((uint32_t)new_value <<  8);
    } else if (destination % 4 == 2) {
        value = value & 0xff00ffff | ((uint32_t)new_value << 16);
    } else if (destination % 4 == 3) {
        value = value & 0x00ffffff | ((uint32_t)new_value << 24);
    }

    // Write the entire word back.
    *destination_aligned = value;
}

void __aeabi_memmove(void *destination, void *source, size_t count)
{
    // The danger is, that we overwride some of the source, before we copy it to the destination.

    uintptr_t destination_address = (uintptr_t)destination;
    uintptr_t source_address = (uintptr_t)source;

    if (destination_address <= source_address) {
        // We can safely copy from left-to-right.

        for (int i = 0; i < count; ++i) {
            unaligned_write_u8(destination_address + i, unaligned_read_u8(source_address + i));
        }
    } else {
        // We can safely copy from right-to-left.

        for (int i = 0; i < count; ++i) {
            unaligned_write_u8(destination_address + count - i - 1, unaligned_read_u8(source_address + count - i - 1));
        }
    }
}
