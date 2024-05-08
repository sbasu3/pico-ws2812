#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812.h"





int main() {
    //set_sys_clock_48();
    stdio_init_all();
    puts("WS2812 parallel");

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    bool current = 0;




    ws2812_program_init(pio, sm, offset, WS2812_PIN_BASE, 800000, false);

    test();

}

