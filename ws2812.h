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


#define NUM_PIXELS_X 8
#define NUM_PIXELS_Y 8
#define TOTAL_PIXELS (NUM_PIXELS_X * NUM_PIXELS_Y)
#define WS2812_PIN_BASE 28


enum dir_t { LEFT_TO_RIGHT = 0, RIGHT_TO_LEFT = 1};

//Contains WS2812 Data in line format
struct pixel_ll{
    uint32_t data[NUM_PIXELS_X];
    bool dir;
    struct pixel_ll *next_ptr;
};

typedef struct pixel_ll pixel_line_t;

//Contains MxN image
typedef struct {
    uint8_t r[NUM_PIXELS_X][NUM_PIXELS_Y];
    uint8_t g[NUM_PIXELS_X][NUM_PIXELS_Y];
    uint8_t b[NUM_PIXELS_X][NUM_PIXELS_Y];
}frame_t;


void pixel_chain_init(pixel_line_t *pixels , bool dir , uint16_t num_pixels);
void pack_pixels(frame_t *frame , pixel_line_t *pixels);



// Helper functions
void copy_row(frame_t *frame, uint start_addr, pixel_line_t *pixels);
void randomize(frame_t *frame);

#endif