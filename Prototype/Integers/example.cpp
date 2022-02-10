// Example 1
{
    extern u8 __data_start__[];
    extern u8 __data_end__[];
    extern u8 __data_lma__[];

    memcpy(__data_start__, __data_lma__, reinterpret_cast<uptr>(__data_end__) - reinterpret_cast<uptr>(__data_start__));
}
{
    extern linker_marker_ptr<u8> __data_start__;
    extern linker_marker_ptr<u8> __data_end__;
    extern linker_marker_ptr<u8> __data_lma__;

    memcpy(__data_start__, __data_lma__, __data_end__.uptr() - __data_start__.uptr());
}

// Example 2
{
    rom_table_lookup_ptr = reinterpret_cast<decltype(rom_table_lookup_ptr)>(static_cast<uptr>(*reinterpret_cast<u16*>(0x00000018)));
    rom_func_table = reinterpret_cast<u16*>(*reinterpret_cast<uptr*>(0x00000014) & 0xffff);
    rom_data_table = reinterpret_cast<u16*>(*reinterpret_cast<uptr*>(0x00000014) >> 16);
}
{
    rom_table_lookup_ptr = ptr<u16>::from(0x00000018)
        .deref()
        .zero_extend_cast<uptr>()
        .pointer_cast<decltype(rom_table_lookup_ptr)>();

    rom_func_table = ptr<u16>::from(0x00000014)
        .deref()
        .zero_extend_cast<uptr>()
        .pointer_cast<ptr<u16>>();

    static_assert(alignof(u16) <= 2);
    rom_data_table = ptr<u16>::from(0x00000016)
        .deref()
        .zero_extend_cast<uptr>()
        .pointer_cast<ptr<u16>>();
}

// Example 3
{
    using init_array_fn = void (*)();
    extern init_array_fn __init_array_start__[];
    extern init_array_fn __init_array_end__[];

    for (auto function = __init_array_start__; function < __init_array_end__; ++function) {
        (*function)();
    }
}
{
    using init_array_fn = void(*)();
    linker_marker_ptr<init_array_fn> __init_array_start__;
    linker_marker_ptr<init_array_fn> __init_array_end__;

    for (isize index = 0; index < __init_array_end__ - __init_array_start__; ++index) {
        __init_array_start__[index]();
    }
}
