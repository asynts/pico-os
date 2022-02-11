export module kit;

export import workaround;

namespace kit {
    export using u8    = unsigned char;
    export using u16   = unsigned short;
    export using u32   = unsigned int;
    export using u64   = unsigned long long;
    export using uptr  = unsigned long;
    export using usize = unsigned int;

    export using i8    = signed char;
    export using i16   = signed short;
    export using i32   = signed int;
    export using i64   = signed long long;
    export using iptr  = signed long;
    export using isize = signed int;

    static_assert(sizeof(uptr) == sizeof(int*));
    static_assert(sizeof(uptr) == sizeof(void(*)()));
    static_assert(sizeof(decltype(sizeof(int))) == sizeof(usize));
}

export
inline void* operator new(kit::usize, void *pointer) {
    return pointer;
}

namespace kit
{
    // FIXME: Export 'assert' and 'assert_not_reached'.

    [[noreturn]]
    void assert_not_reached() {
        for (;;) {
            asm volatile("bkpt #0");
        }
    }

    void assert(bool condition) {
        if (!condition) {
            for (;;) {
                asm volatile("bkpt #0");
            }
        }
    }

    export
    template<typename T>
    struct Singleton {
    private:
        static T m_value;
        static bool m_initialized;

    public:
        static void initialize() {
            if (m_initialized) {
                assert_not_reached();
            } else {
                new (&m_value) T;
                m_initialized = true;
            }
        }

        static T& the() {
            if (m_initialized) {
                return m_value;
            } else {
                assert_not_reached();
            }
        }
    };
}
