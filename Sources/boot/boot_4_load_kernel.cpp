export module boot;

[[gnu::section(".noinit")]]
void* (*memcpy_ptr)(void *destination, const void *source, size_t count) = nullptr;

export extern "C"
void* memcpy(void *destination, const void *source, size_t count) {
    return memcpy_ptr(destination, source, count);
}

[[gnu::section(".noinit")]]
void* (*memset_ptr)(void *destination, int character, size_t count) = nullptr;

export extern "C"
void* memset(void *destination, int character, size_t count) {
    return memset(destination, character, count);
}

export
uint32_t rom_func_lookup(char ch1, char ch2) {
    // FIXME: Magic constant.
    auto rom_func_table = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(*reinterpret_cast<uint16_t*>(0x00000014)));
    return rom_func_table[ch2 << 8 | ch 1];
}

// FIXME: Move this into another C++ module.
namespace kernel
{
    void entry() {
        asm volatile("bkpt #0");
    }
}

export extern "C"
void boot_4_load_kernel() {
    // First, we lookup the addresses for common helper functions.
    // They are used by the corresponding functions.
    // These pointers are in a special '.noinit' section, otherwise, the calls below would override them.
    memcpy_ptr = reinterpret_cast<decltype(memcpy_ptr)>(rom_func_lookup('M', 'C'));
    memset_ptr = reinterpret_cast<decltype(memset_ptr)>(rom_func_lookup('M', 'S'));

    // Load the '.data' section into memory.
    memcpy(__data_start__, /* FIXME */, __data_end__ - __data_start__);

    // Clear the '.bss' section.
    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    kernel::entry();
}
