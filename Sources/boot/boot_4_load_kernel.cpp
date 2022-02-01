export module boot;

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
void* (*memcpy_ptr)(void *destination, const void *source, usize count) = nullptr;

export extern "C"
void* memcpy(void *destination, const void *source, usize count) {
    return memcpy_ptr(destination, source, count);
}

[[gnu::section(".noinit")]]
void* (*memset_ptr)(void *destination, int character, usize count) = nullptr;

export extern "C"
void* memset(void *destination, int character, usize count) {
    return memset(destination, character, count);
}

export
u32 rom_func_lookup(char ch1, char ch2) {
    // FIXME: Magic constant.
    auto rom_func_table = reinterpret_cast<u32*>(static_cast<uptr>(*reinterpret_cast<u16*>(0x00000014)));
    return rom_func_table[ch2 << 8 | ch1];
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

    // First, we lookup the addresses for common helper functions.
    // They are used by the corresponding functions.
    // These pointers are in a special '.noinit' section, otherwise, the calls below would override them.
    memcpy_ptr = reinterpret_cast<decltype(memcpy_ptr)>(rom_func_lookup('M', 'C'));
    memset_ptr = reinterpret_cast<decltype(memset_ptr)>(rom_func_lookup('M', 'S'));

    // Load the '.data' section into memory.
    memcpy(__data_start__, __data_lma__, __data_end__ - __data_start__);

    // Clear the '.bss' section.
    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    // FIXME: .init_array section.

    kernel::entry();
}
