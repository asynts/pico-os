#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/printf.h>
#include <string.h>

void wait_for_host()
{
    char buffer[16] = {0};
    fgets(buffer, sizeof(buffer), stdin);
    if (strcmp(buffer, "connect\r\n") != 0)
        panic("Expected connection sequence, received '%s'", buffer);
}

int main() {
    stdio_init_all();
    wait_for_host();

    gpio_init(25);
    gpio_set_dir(25, true);

    for (;;) {
        gpio_xor_mask(1 << 25);
        sleep_ms(100);
        gpio_xor_mask(1 << 25);
        sleep_ms(100);
    }
}

// Note. I patched the pico-sdk and made _exit a weak symbol.
extern "C"
void _exit(int)
{
    gpio_init(25);
    gpio_set_dir(25, true);
    gpio_put(25, true);

    for(;;)
        asm("wfi");
}
