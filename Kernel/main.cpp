#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/printf.h>
#include <string.h>

#include <Kernel/Scheduler.hpp>
#include <Std/Vector.hpp>

[[noreturn]]
void blink_task(void)
{
    gpio_init(25);
    gpio_set_dir(25, true);

    for(;;) {
        gpio_xor_mask(1 << 25);
        sleep_ms(100);
    }
}

[[noreturn]]
void message_task(void)
{
    usize index = 0;
    for(;;) {
        printf("Message #%d\n", index++);
        sleep_ms(150);
    }
}

int main() {
    stdio_init_all();

    Kernel::Scheduler::the();

    Kernel::Scheduler::the().create_task(blink_task);
    Kernel::Scheduler::the().create_task(message_task);

    Kernel::Scheduler::the().loop();
}

// Note. I patched the pico-sdk and made _exit a weak symbol.
extern "C"
void _exit(int)
{
    gpio_init(25);
    gpio_set_dir(25, true);
    gpio_put(25, true);

    for(;;)
        __wfi();
}
