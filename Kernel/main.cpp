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
        gpio_xor_mask(1 << 25);

        // FIXME: We want to be able to set a timer in a task and regain control
        //        when it runs out.
        for (usize i = 0; i < 1000000; ++i)
            __nop();
    }
}

[[noreturn]]
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

    printf("Booting...\n");

    Kernel::Scheduler::the();

    Kernel::Task *blink_task_ptr = Kernel::Scheduler::the().create_task(blink_task);
    printf("blink_task_ptr: %p\n", blink_task_ptr);
    Kernel::Task *message_task_ptr = Kernel::Scheduler::the().create_task(message_task);
    printf("message_task_ptr: %p\n", message_task_ptr);

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
