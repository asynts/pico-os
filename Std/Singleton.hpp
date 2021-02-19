#pragma once

#include <pico/mutex.h>

#include <Std/Forward.hpp>

namespace Std {
    auto_init_mutex(singleton_mutex);

    template<typename T>
    class Singleton {
    public:
        static T& the()
        {
            static bool initialized = false;
            static u8 instance[sizeof(T)];

            if (initialized)
                return *reinterpret_cast<T*>(instance);

            mutex_enter_blocking(&singleton_mutex);

            if (initialized)
                return *reinterpret_cast<T*>(instance);

            new (instance) T;

            mutex_exit(&singleton_mutex);

            return *reinterpret_cast<T*>(instance);
        }
    };
}
