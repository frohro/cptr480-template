#include "pico/stdlib.h"

// ---------------------------------------------------------------------------
// Lab 1 — SDK Blink
//
// TODO: Replace LED_PIN with the correct GPIO number for one of the 2026
// board's status LEDs. Find it by tracing the LED net in the schematic.
//
// Default (25) drives the Pico's onboard LED — useful for toolchain verify
// before you've read the schematic.
// ---------------------------------------------------------------------------
#define LED_PIN 25

int main(void) {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }

    return 0;
}
