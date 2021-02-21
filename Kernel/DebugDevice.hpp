#pragma once

#include <hardware/gpio.h>
#include <pico/printf.h>

#include <Std/Singleton.hpp>

constexpr uint status_led_pin = 25;

namespace Kernel {
    using namespace Std;

    class DebugDevice : public Singleton<DebugDevice> {
    public:
        [[noreturn]]
        void crash(const char *filename, usize line)
        {
            gpio_init(status_led_pin);
            gpio_set_dir(status_led_pin, true);
            gpio_put(status_led_pin, true);

            printf("Crash at %s:%zu\n", filename, line);

            for(;;)
                asm("wfi");
        }
    };
}
