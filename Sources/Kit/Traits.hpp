#pragma once

namespace Kit::Traits
{
    template<typename T>
    struct RemoveConst_ {
        using Type = T;
    };
    template<typename T>
    struct RemoveConst_<const T> {
        using Type = T;
    };
    template<typename T>
    using RemoveConst = typename RemoveConst_<T>::Type;
}
