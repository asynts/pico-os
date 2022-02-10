// FIXME: constexpr
// FIXME: test all of this rigerously
// FIXME: protect pointers with 'deref'

namespace traits
{
    template<typename T>
    struct is_primitive_signed {
        static_assert(false);
    };
    template<>
    struct is_primitive_signed<unsigned char> {
        static constexpr bool value = false;
    };
    template<>
    struct is_primitive_signed<unsigned short> {
        static constexpr bool value = false;
    };
    template<>
    struct is_primitive_signed<unsigned int> {
        static constexpr bool value = false;
    };
    template<>
    struct is_primitive_signed<unsigned long> {
        static constexpr bool value = false;
    };
    template<>
    struct is_primitive_signed<unsigned long long> {
        static constexpr bool value = false;
    };
    template<>
    struct is_primitive_signed<signed char> {
        static constexpr bool value = true;
    };
    template<>
    struct is_primitive_signed<signed short> {
        static constexpr bool value = true;
    };
    template<>
    struct is_primitive_signed<signed int> {
        static constexpr bool value = true;
    };
    template<>
    struct is_primitive_signed<signed long> {
        static constexpr bool value = true;
    };
    template<>
    struct is_primitive_signed<signed long long> {
        static constexpr bool value = true;
    };
}

namespace concepts {

}

template<typename T>
struct alignas(T) non_promoting_integer
{
private:
    T m_value;

public:
    static bool is_signed = traits::is_primitive_signed_v<T>;

    explicit non_promoting_integer(T value)
        : m_value(value) { }

    T value() const {
        return m_value;
    }

    // FIXME: Pointer

    template<typename S>
    static non_promoting_integer from(S value)
    requires traits::is_primitive_integer_v<S> && traits::is_primitive_signed_v<T> == traits::is_primitive_signed_v<S>
    {
        return non_promoting_integer { static_cast<T>(value) };
    }

    template<typename S>
    static non_promoting_integer from(non_promoting_integer<S> value)
    requires sizeof(T) >= sizeof(S) && traits::is_primitive_signed_v<T> == traits::is_primitive_signed_v<S>
    {
        return non_promoting_integer { static_cast<T>() }
    }

    non_promoting_integer operator+(non_promoting_integer rhs) const {
        return non_promoting_integer{ m_value + rhs };
    }
    non_promoting_integer operator-(non_promoting_integer rhs) const {
        return non_promoting_integer{ m_value - rhs };
    }

    non_promoting_integer& operator+=(non_promoting_integer rhs) {
        m_value += rhs;
        return *this;
    }
    non_promoting_integer& operator-=(non_promoting_integer rhs) {
        m_value -= rhs;
        return *this;
    }

    non_promoting_integer& operator=(non_promoting_integer rhs) {
        m_value = rhs.m_value;
        return *this;
    }
};

using u8 = non_promoting_integer<unsigned char>;

using u16 = non_promoting_integer<unsigned short>;

using u32 = non_promoting_integer<unsigned int>;

using u64 = non_promoting_integer<unsigned long long>;

using usize = u64;
static_assert(sizeof(usize) == sizeof(decltype(sizeof(int))));

using uptr = u64;
static_assert(sizeof(uptr) == sizeof(int*));
static_assert(sizeof(uptr) == sizeof(void(*)()));

int main() {
    u16 a = u16::from(14);
    u32 b = u32::from(13);

    u32 c = a + b;
}
