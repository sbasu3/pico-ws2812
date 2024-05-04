#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812.h"



#define NUM_BUFFER 2


uint32_t **screen;

frame_t image;

static char *header_data =
	"````````````````````````````````````````````````````````````````"
	"````````]/OI````````]/OI````````````````]/OI````````]/OI````````"
	"`-C.`^?D`````````````````^?D`-C.`^?D`-C.`^?D`````````^?D`-C.`^?D"
	"`````^?D`-C.`^?D`^?D`-C.`^?D`````````````^?D`-C.`-C.`^?D````````"
	"";




int main() {
    //set_sys_clock_48();
    stdio_init_all();
    puts("WS2812 parallel");

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    bool current = 0;


    screen = malloc(sizeof(uint32_t *)*NUM_PIXELS_Y);
    for(int i =0; i < NUM_PIXELS_X; i++)
        screen[i] = malloc(sizeof(uint32_t)* NUM_PIXELS_X);

    pixel_reset(screen, 0x0);
    //pixel_reset((uint32_t **)screen[1], 0x0);

    ws2812_program_init(pio, sm, offset, WS2812_PIN_BASE, 800000, false);

    test();

}

