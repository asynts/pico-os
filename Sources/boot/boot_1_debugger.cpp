// FIXME: Make this work with C++20 modules.
// module boot;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using rom_table_lookup_fn = void*(*)(u16 *table, u32 code);
using rom_memcpy44_fn = u8*(*)(u32 *destination, u32 *source, u32 count);

extern "C"
void boot_2_flash_second_stage() noexcept;

extern "C"
__attribute__((section(".boot_1_debugger")))
void boot_1_debugger() noexcept {
    u32 data;

    // FIXME: The compiler appears to translate this into an unaligned load instrutions.
    //        Therefore, we hard-fault.

    data = *reinterpret_cast<u32*>(0x00000012);
    u32 rom_func_table_ptr = ((data >> 8) & 0xff00) | ((data >> 24) & 0xff);
    auto rom_func_table = reinterpret_cast<u16*>(rom_func_table_ptr);

    data = *reinterpret_cast<u32*>(0x00000016);
    u32 rom_table_lookup_ptr = ((data >> 8) & 0xff00) | ((data >> 24) & 0xff);
    auto rom_table_lookup = reinterpret_cast<rom_table_lookup_fn>(rom_table_lookup_ptr);

    auto rom_memcpy44 = reinterpret_cast<rom_memcpy44_fn>(
        rom_table_lookup(rom_func_table, static_cast<u32>('C') | (static_cast<u32>('4') << 8))
    );

    // Manually load the first 256 bytes of flash into RAM.
    // Usually, this is done by the reset logic in ROM.
    rom_memcpy44(reinterpret_cast<u32*>(0x20000000), reinterpret_cast<u32*>(0x10000000), 0x100);

    boot_2_flash_second_stage();
}
