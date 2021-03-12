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
}

namespace Std::Concepts {
    template<typename T, typename S>
    concept Same = IsSame<T, S>::value;

    template<typename T>
    concept Integral = Same<T, u8>
                    || Same<T, u16>
                    || Same<T, u32>
                    || Same<T, u64>
                    || Same<T, i8>
                    || Same<T, i16>
                    || Same<T, i32>
                    || Same<T, i64>;
}
