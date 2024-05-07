#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ws2812.h"

void printPixels(uint32_t **pixels) {
    int row, col;
    for (row = 0; row < NUM_PIXELS_X; row++) {
        for (col = 0; col < NUM_PIXELS_Y; col++) {
            printf("%08X ", pixels[row][col]);
        }
        printf("\n");
    }
}

int test() {
    // Seed the random number generator
    srand(time(NULL));

    // Create a frame and pixel array
    frame_t frame;
    uint32_t **pixels = (uint32_t **)malloc(NUM_PIXELS_X * sizeof(uint32_t *));
    int row;
    for (row = 0; row < NUM_PIXELS_X; row++) {
        pixels[row] = (uint32_t *)malloc(NUM_PIXELS_Y * sizeof(uint32_t));
    }
    /*
    // Test pixel_reset function
    printf("Testing pixel_reset function:\n");
    pixel_reset(pixels, 0xFF0000);
    printPixels(pixels);

    // Test pack_pixels function
    printf("\nTesting pack_pixels function:\n");
    randomize(&frame);
    pack_pixels(&frame, pixels);
    printPixels(pixels);

    // Test copy_row function
    printf("\nTesting copy_row function:\n");
    copy_row(&frame, 0, pixels);
    printPixels(pixels);

    // Test read_data function
    printf("\nTesting read_data function:\n");
    char data[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
    read_data(&frame, data);
    printPixels(pixels);
    */
    // Test blink_effect function
    printf("\nTesting blink_effect function:\n");
    blink_effect(&frame, pixels, 0xFF0000, 0x00FF00, 2000);
    printPixels(pixels);

    // Test fade_effect function
    printf("\nTesting fade_effect function:\n");
    fade_effect(&frame, pixels, 0x34ebd2, 4000, 16);
    printPixels(pixels);
    /*
    // Test randomized_effect function
    printf("\nTesting randomized_effect function:\n");
    randomized_effect(&frame, pixels, 2000);
    printPixels(pixels);

    // Test solid_color_effect function
    printf("\nTesting solid_color_effect function:\n");
    solid_color_effect(&frame, pixels, 0x0000FF);
    printPixels(pixels);

    // Test clear_effect function
    printf("\nTesting clear_effect function:\n");
    clear_effect(&frame, pixels);
    printPixels(pixels);

    // Test running_light_effect function
    printf("\nTesting running_light_effect function:\n");
    running_light_effect(&frame, pixels, 0xFF00FF, 2000);
    printPixels(pixels);

    // Test fade_in_out_effect function
    printf("\nTesting fade_in_out_effect function:\n");
    fade_in_out_effect(&frame, pixels, 0xFF0000, 4, 4, 2000);
    printPixels(pixels);
    */
    // Free the allocated memory
    for (row = 0; row < NUM_PIXELS_X; row++) {
        free(pixels[row]);
    }
    free(pixels);

    return 0;
}