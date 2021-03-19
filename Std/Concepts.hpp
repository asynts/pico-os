#pragma once

#include <Std/Forward.hpp>

namespace Std {
    template<typename T, typename S>
    struct IsSame {
        static constexpr bool value = false;
    };
    template<typename T>
    struct IsSame<T, T> {
        static constexpr bool value = true;
    };

    template<typename T>
    struct RemoveConst {
        using Type = T;
    };
    template<typename T>
    struct RemoveConst<const T> {
        using Type = T;
    };
}

namespace Std::Concepts {
    template<typename T, typename S>
    concept Same = IsSame<T, S>::value;

    template<typename T>
    concept Integral = Same<T, unsigned char>
                    || Same<T, unsigned short>
                    || Same<T, unsigned int>
                    || Same<T, unsigned long>
                    || Same<T, unsigned long long>
                    || Same<T, signed char>
                    || Same<T, signed short>
                    || Same<T, signed int>
                    || Same<T, signed long>
                    || Same<T, signed long long>;
}
