#include <pico/stdlib.h>

constexpr uint status_led_pin = 25;

int main() {
    gpio_init(status_led_pin);
    gpio_set_dir(status_led_pin, true);
    gpio_put(status_led_pin, true);
}
