#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    for(;;)
        __wfi();
}
