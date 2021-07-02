#pragma once

#include <Std/Forward.hpp>

namespace Std {
    template<typename T>
    class Singleton {
    public:
        Singleton(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;

        Singleton() = default;

        static T& the()
        {
            static bool initialized = false;
            static u8 instance[sizeof(T)];

            if (initialized)
                return *reinterpret_cast<T*>(instance);

            initialized = true;
            new (instance) T;

            return *reinterpret_cast<T*>(instance);
        }
    };
}
