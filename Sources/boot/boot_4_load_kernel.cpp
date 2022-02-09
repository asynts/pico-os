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

// Almost all functions defined here rely on initialization logic in 'boot_4_load_kernel'.
// They should be used with extreme care, but should be safe to use in 'boot_5_kernel_entry'.

[[gnu::section(".noinit")]]
void* (*memcpy_ptr)(void *destination, const void *source, u32 count) = nullptr;

export extern "C"
void __aeabi_memcpy(void *destination, const void *source, usize count) {
    memcpy_ptr(destination, source, count);
}

export extern "C"
void* memcpy(void *destination, const void *source, usize count) {
    __aeabi_memcpy(destination, source, count);
    return destination;
}

[[gnu::section(".noinit")]]
void* (*memset_ptr)(void *destination, u8 fill, u32 count) = nullptr;

export extern "C"
void __aeabi_memset(void *destination, usize count, int fill) {
    memset_ptr(destination, static_cast<u8>(fill), count);
}

export extern "C"
void* memset(void *destination, int fill, usize count) {
    __aeabi_memset(destination, count, fill);
    return destination;
}

[[gnu::section(".noinit")]]
u32 (*rom_table_lookup_ptr)(u16 *table, u32 code);

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
export extern "C"
void boot_5_kernel_entry() {
    asm volatile("bkpt #0");
}

// Defined by linker script.
extern u8 __data_start__[];
extern u8 __data_end__[];
extern u8 __bss_start__[];
extern u8 __bss_end__[];
extern u8 __data_lma__[];

using init_array_fn = void (*)();
extern init_array_fn __init_array_start__[];
extern init_array_fn __init_array_end__[];

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

    // Call all the global constructors.
    // This should not be used in this codebase, but we'll add support for it anyways.
    for (auto function = __init_array_start__; function < __init_array_end__; ++function) {
        (*function)();
    }

    boot_5_kernel_entry();
}
