export module kit.integers;

namespace kit::integers {
    // FIXME: constexpr
    // FIXME: test all of this rigerously
    // FIXME: protect pointers with 'deref'

    export
    template<typename T>
    struct alignas(T) non_promoting_integer
    {
    private:
        T m_value;

    public:
        explicit non_promoting_integer(T value)
            : m_value(value) { }

        T value() const {
            return m_value;
        }

        template<typename S>
        S cast() const; // FIXME

        template<typename S>
        static non_promoting_integer from(S value) {
            return non_promoting_integer { static_cast<T>(value) };
        }

        non_promoting_integer operator+(non_promoting_integer rhs) const {
            return non_promoting_integer{ m_value + rhs };
        }
        non_promoting_integer operator-(non_promoting_integer rhs) const {
            return non_promoting_integer{ m_value - rhs };
        }
        non_promoting_integer operator*(non_promoting_integer rhs) const {
            return non_promoting_integer{ m_value * rhs };
        }
        non_promoting_integer operator/(non_promoting_integer rhs) const {
            return non_promoting_integer{ m_value / rhs };
        }

        // FIXME: This is suboptimal
        non_promoting_integer operator<<(unsigned int rhs) const {
            return non_promoting_integer{ m_value << rhs };
        }
        non_promoting_integer operator>>(unsigned int rhs) const {
            return non_promoting_integer{ m_value >> rhs };
        }

        non_promoting_integer operator|(non_promoting_integer rhs) const {
            return non_promoting_integer { m_value | rhs };
        }
        non_promoting_integer operator&(non_promoting_integer rhs) const {
            return non_promoting_integer { m_value & rhs };
        }

        non_promoting_integer& operator+=(non_promoting_integer rhs) {
            m_value += rhs;
            return *this;
        }
        non_promoting_integer& operator-=(non_promoting_integer rhs) {
            m_value -= rhs;
            return *this;
        }
        non_promoting_integer& operator/=(non_promoting_integer rhs) {
            m_value /= rhs;
            return *this;
        }
        non_promoting_integer& operator*=(non_promoting_integer rhs) {
            m_value *= rhs;
            return *this;
        }
        non_promoting_integer& operator<<=(non_promoting_integer rhs) {
            m_value <<= rhs;
            return *this;
        }
        non_promoting_integer& operator>>=(non_promoting_integer rhs) {
            m_value >>= rhs;
            return *this;
        }
        non_promoting_integer& operator|=(non_promoting_integer rhs) {
            m_value |= rhs;
            return *this;
        }
        non_promoting_integer& operator&=(non_promoting_integer rhs) {
            m_value &= rhs;
            return *this;
        }
    };

    namespace prelude {
        export
        using u8 = non_promoting_integer<unsigned char>;

        export
        using u16 = non_promoting_integer<unsigned short>;

        export
        using u32 = non_promoting_integer<unsigned int>;

        export
        using u64 = non_promoting_integer<unsigned long long>;

        export
        using usize = u32;
        static_assert(sizeof(usize) == sizeof(decltype(sizeof(int))));

        export
        using uptr = u32;
        static_assert(sizeof(uptr) == sizeof(int*));
        static_assert(sizeof(uptr) == sizeof(void(*)()));
    }
    using namespace prelude;
}
