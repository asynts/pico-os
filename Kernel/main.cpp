#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/printf.h>

#include <Kernel/MemoryAllocator.hpp>

int main() {
    stdio_init_all();

    // Give the host some time to connect to debug output.
    sleep_ms(2000);

    Kernel::MemoryAllocator::the();

    gpio_init(25);
    gpio_set_dir(25, true);

    for (;;) {
        gpio_xor_mask(1 << 25);
        sleep_ms(100);
        gpio_xor_mask(1 << 25);
        sleep_ms(100);
    }
}
