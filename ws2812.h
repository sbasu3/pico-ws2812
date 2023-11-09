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

//Contains WS2812 Data in 2D array format
//uint32_t pixel[NUM_PIXELS_X][NUM_PIXELS_Y];

//Contains MxN image
typedef struct {
    uint8_t r[NUM_PIXELS_X][NUM_PIXELS_Y];
    uint8_t g[NUM_PIXELS_X][NUM_PIXELS_Y];
    uint8_t b[NUM_PIXELS_X][NUM_PIXELS_Y];
}frame_t;


void pixel_reset(uint32_t **pixels, uint32_t val);
void pack_pixels(frame_t *frame ,uint32_t  **pixels);

void read_data(frame_t *frame, char * data);


// Helper functions
void copy_row(frame_t *frame, uint row, uint32_t **pixels);
void randomize(frame_t *frame);

#endif