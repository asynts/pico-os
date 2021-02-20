#include <pico/stdlib.h>

#include <Kernel/PageAllocator.hpp>
#include <Kernel/MemoryAllocator.hpp>
#include <Kernel/DebugDevice.hpp>

constexpr uint status_led_pin = 25;

int main() {
    Kernel::MemoryAllocator::the();
    Kernel::PageAllocator::the();

    gpio_init(status_led_pin);
    gpio_set_dir(status_led_pin, true);
    gpio_put(status_led_pin, true);
}
