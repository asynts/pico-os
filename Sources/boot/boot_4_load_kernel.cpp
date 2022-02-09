export module boot;

// FIXME: Setup wrapper types that protect against implicit conversions.
//        There should be unit test that verify the functionality of that library.

using u8   = unsigned char;
using u16  = unsigned short;
using u32  = unsigned int;
using u64  = unsigned long long;
using uptr = unsigned long;
using usize = unsigned long;

using i8   = signed char;
using i16  = signed short;
using i32  = signed int;
using i64  = signed long long;
using iptr = signed long;
using isize = signed long;

static_assert(sizeof(uptr) == sizeof(int*));
static_assert(sizeof(uptr) == sizeof(void(*)()));

[[gnu::section(".noinit")]]
void* (*memcpy_ptr)(void *destination, const void *source, u32 count) = nullptr;

// Relies on initialization logic in 'boot_4_load_kernel'.
export extern "C"
void* memcpy(void *destination, const void *source, usize count) {
    return memcpy_ptr(destination, source, count);
}

[[gnu::section(".noinit")]]
void* (*memset_ptr)(u8 *destination, u8 character, u32 count) = nullptr;

// Relies on initialization logic in 'boot_4_load_kernel'.
export extern "C"
void* memset(void *destination, int character, usize count) {
    // FIXME: Is this the correct cast?
    // FIXME: We hang here for some reason.
    return memset(destination, static_cast<u8>(character), count);
}

[[gnu::section(".noinit")]]
u32 (*rom_table_lookup_ptr)(u16 *table, u32 code);

// Relies on initialization logic in 'boot_4_load_kernel'.
export
u32 rom_table_lookup(u16 *table, u32 code) {
    return rom_table_lookup_ptr(table, code);
}

[[gnu::section(".noinit")]]
u16 *rom_func_table;

export
u32 rom_func_lookup(char ch1, char ch2) {
    return rom_table_lookup(rom_func_table, (ch2 << 8) | ch1);
}

[[gnu::section(".noinit")]]
u16 *rom_data_table;

export
u32 rom_data_lookup(char ch1, char ch2) {
    return rom_table_lookup(rom_data_table, (ch2 << 8) | ch1);
}

// FIXME: Move this into another C++ module.
namespace kernel
{
    void entry() {
        asm volatile("bkpt #0");
    }
}

// Defined by linker script.
extern u8 __data_start__[];
extern u8 __data_end__[];
extern u8 __bss_start__[];
extern u8 __bss_end__[];
extern u8 __data_lma__[];

export extern "C"
void boot_4_load_kernel() {
    asm volatile("bkpt #0");

    // First, we lookup the addresses that are required to use the 'rom_func_lookup' and 'rom_data_lookup' functions.
    rom_table_lookup_ptr = reinterpret_cast<decltype(rom_table_lookup_ptr)>(static_cast<uptr>(*reinterpret_cast<u16*>(0x00000018)));
    rom_func_table = reinterpret_cast<u16*>(*reinterpret_cast<uptr*>(0x00000014) & 0xffff);
    rom_data_table = reinterpret_cast<u16*>(*reinterpret_cast<uptr*>(0x00000014) >> 16);

    // Then, we can use these helpers to find all the other functions that are defined in ROM.
    memcpy_ptr = reinterpret_cast<decltype(memcpy_ptr)>(rom_func_lookup('M', 'C'));
    memset_ptr = reinterpret_cast<decltype(memset_ptr)>(rom_func_lookup('M', 'S'));

    // Notice, that we placed all the pointers above in a special '.noinit' sections such that we won't override them by accident.

    // Load the '.data' section into memory.
    memcpy(__data_start__, __data_lma__, __data_end__ - __data_start__);

    // Clear the '.bss' section.
    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    // FIXME: .init_array section.

    kernel::entry();
}
