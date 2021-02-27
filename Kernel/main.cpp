#include <hardware/gpio.h>
#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>
#include <hardware/structs/systick.h>

#include <Kernel/Scheduler.hpp>

// FIXME: This should be in the Pico SDK!
__attribute__((weak))
void __nop()
{
    asm volatile("nop");
}

void returning_task()
{
    printf("Hi, I am the task that returns!\n");
}

void blink_task()
{
    gpio_init(25);
    gpio_set_dir(25, true);

    for(;;) {
        gpio_xor_mask(1 << 25);

        printf("Blink!, btw. systick_hw->cvr=%zu, systick_hw->csr=%zb\n", systick_hw->cvr, systick_hw->csr);

        // FIXME: We want to be able to set a timer in a task and regain control
        //        when it runs out.
        for (usize i = 0; i < 1000000; ++i)
            __nop();
    }
}

void message_task()
{
    usize index = 0;
    for(;;) {
        printf("Message #%zu\n", index++);

        for (usize i = 0; i < 1000000; ++i)
            __nop();
    }
}

int main() {
    stdio_init_all();

    printf("\n\033[1mBOOT\033[0m\n");

    Kernel::Scheduler::the();

    Kernel::Scheduler::the().create_task(returning_task);
    // Kernel::Scheduler::the().create_task(blink_task);
    // Kernel::Scheduler::the().create_task(message_task);

    Kernel::Scheduler::the().loop();
}

extern "C"
void _exit(int)
{
    gpio_init(25);
    gpio_set_dir(25, true);
    gpio_put(25, true);

    for(;;)
        __wfi();
}
