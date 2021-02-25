#include <hardware/gpio.h>
#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>

#include <Kernel/Scheduler.hpp>

// FIXME: This should be in the Pico SDK!
__attribute__((weak))
void __nop()
{
    asm volatile("nop");
}

[[noreturn]]
void blink_task()
{
    gpio_init(25);
    gpio_set_dir(25, true);

    for(;;) {
        printf("Blink!\n");
        gpio_xor_mask(1 << 25);

        // FIXME: We want to be able to set a timer in a task and regain control
        //        when it runs out.
        for (usize i = 0; i < 1000000; ++i)
            __nop();
    }
}

int main() {
    stdio_init_all();

    printf("Booting...\n");

    Kernel::Scheduler::the();

    Kernel::Scheduler::the().create_task(blink_task);

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
