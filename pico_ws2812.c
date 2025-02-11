#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812.h"

#define WS2812_PIN_BASE 10



int main() {
    //set_sys_clock_48();
    stdio_init_all();
    puts("WS2812 parallel");

    ws2812_t led_chain;	

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    bool current = 0;
    uint freq = 800000;




   //ws2812_program_init(pio, sm, offset, WS2812_PIN_BASE, 800000, false);
    ws2812_init(&led_chain,pio,sm,WS2812_PIN_BASE,freq);
    
    test(&led_chain);

}

