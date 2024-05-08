#ifndef __WS2812__
#define __WS2812__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/sem.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "ws2812.pio.h"
#include "hardware/clocks.h"
#include "time.h"
#include "ws2812.pio.h"


#define PIXELS_PER_SEG 8
#define NUM_SEG 2
#define TOTAL_PIXELS (PIXEL_PER_SEG * NUM_SEG)
#define WS2812_PIN_BASE 2
// Helper functions
// color contains RGB data
uint8_t extract_r(uint32_t color);
uint8_t extract_g(uint32_t color);
uint8_t extract_b(uint32_t color);

typedef enum{
    BLINK = 0,
    FADE,
    RANDOMIZED,
    SOLID_COLOR,
    CLEAR,
    RUNNING_LIGHT,
    FADE_IN_OUT
}effect_t;

//Contains a pixel
typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
}pixel_t;

//pixel functions
void set_pixel(pixel_t *pixel, uint32_t color);




// WS2812 functions
typedef enum{
    ORIENTATION_UP = 0,
    ORIENTATION_DOWN
}orientation_t;

typedef struct{
    pixel_t pixels[PIXELS_PER_SEG];
}segment_t;

void set_segment(segment_t *segment, orientation_t dir,uint32_t *color,uint8_t num_pixels);



typedef struct{
    segment_t segments[NUM_SEG];
    orientation_t orientation[NUM_SEG];
}ws2812_t;

void ws2812_init(ws2812_t *ws2812);
void ws2812_show(ws2812_t *ws2812);
void ws2812_clear(ws2812_t *ws2812);
void ws2812_set_pixel(ws2812_t *ws2812, uint8_t seg, uint8_t pixel, uint32_t color);
void ws2812_set_segment(ws2812_t *ws2812, uint8_t seg, orientation_t dir, uint32_t *color, uint8_t num_pixels);

void ws2812_blink(ws2812_t *ws2812, uint32_t color1, uint32_t color2, uint32_t delay_ms);
void ws2812_fade(ws2812_t *ws2812, uint32_t color1, uint32_t color2, uint32_t delay_ms);
void ws2812_solid_color(ws2812_t *ws2812, uint32_t color);
void ws2812_running_light(ws2812_t *ws2812, uint32_t color, uint32_t delay_ms);
void ws2812_fade_in_out(ws2812_t *ws2812, uint32_t color, uint32_t delay_ms);


int test();

#endif