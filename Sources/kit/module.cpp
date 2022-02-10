export module kit;

namespace kit {
    namespace prelude {
        export using u8    = unsigned char;
        export using u16   = unsigned short;
        export using u32   = unsigned int;
        export using u64   = unsigned long long;
        export using uptr  = unsigned long;
        export using usize = unsigned long;

        export using i8    = signed char;
        export using i16   = signed short;
        export using i32   = signed int;
        export using i64   = signed long long;
        export using iptr  = signed long;
        export using isize = signed long;

        static_assert(sizeof(uptr) == sizeof(int*));
        static_assert(sizeof(uptr) == sizeof(void(*)()));
        static_assert(sizeof(decltype(sizeof(int))) == sizeof(usize));
    }
    using namespace prelude;
}
