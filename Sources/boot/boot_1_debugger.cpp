// FIXME: Make this work with C++20 modules.
// FIXME: Make this work without 'noexcept' everywhere.
// module boot;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using rom_table_lookup_fn = void*(*)(u16 *table, u32 code);
using rom_memcpy44_fn = u8*(*)(u32 *destination, u32 *source, u32 count);

extern "C"
void boot_2_flash_second_stage() noexcept;

#define ASSERT(condition) \
    { \
        if (!(condition)) { \
            asm volatile("bkpt #0"); \
        } \
    }

extern "C"
__attribute__((section(".boot_1_debugger")))
void boot_1_debugger() noexcept {
    u32 data;

    data = *reinterpret_cast<u32*>(0x00000014);
    u16 rom_func_table_ptr = static_cast<u16>(data);
    auto rom_func_table = reinterpret_cast<u16*>(rom_func_table_ptr);

    // This may be a false positive.
    ASSERT(rom_func_table_ptr == 0x007a);

    data = *reinterpret_cast<u32*>(0x00000018);
    u16 rom_table_lookup_ptr = static_cast<u16>(data);
    auto rom_table_lookup = reinterpret_cast<rom_table_lookup_fn>(rom_table_lookup_ptr);

    // This may be a false positive.
    ASSERT(rom_table_lookup_ptr == 0x001d);

    auto rom_memcpy44 = reinterpret_cast<rom_memcpy44_fn>(
        rom_table_lookup(rom_func_table, static_cast<u32>('C') | (static_cast<u32>('4') << 8))
    );

    // Manually load the first 256 bytes of flash into RAM.
    // Usually, this is done by the reset logic in ROM.
    rom_memcpy44(reinterpret_cast<u32*>(0x20000000), reinterpret_cast<u32*>(0x10000000), 0x100);

    boot_2_flash_second_stage();
}
