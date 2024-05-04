/**
 * @file ws2812.c
 * @brief Implementation of functions for controlling WS2812 LED strip.
 */

#include "ws2812.h"

static unsigned int width = 8; /**< Width of the LED strip */
static unsigned int height = 8; /**< Height of the LED strip */

/**
 * @brief Macro to extract pixel data from input data.
 * @param data Pointer to the input data.
 * @param pixel Array to store the extracted pixel data.
 */
#define HEADER_PIXEL(data,pixel) {\
    pixel[2] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
    pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
    pixel[0] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
    data += 4; \
}
void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}
/**
 * @brief Resets all pixels in the LED strip to a specified value.
 * @param pixels Pointer to the 2D array representing the LED strip.
 * @param val Value to set all pixels to.
 */
void pixel_reset(uint32_t **pixels , uint32_t val){
    uint row,col;
    for(row = 0 ; row < NUM_PIXELS_X ; row++)
        for(col = 0; col < NUM_PIXELS_Y ; col++)
            pixels[row][col] = val;
}

/**
 * @brief Packs the pixel data from the frame into the LED strip.
 * @param frame Pointer to the frame containing the pixel data.
 * @param pixels Pointer to the 2D array representing the LED strip.
 */
void pack_pixels(frame_t *frame ,uint32_t **pixels){
    uint row,col;
    for( row = 0; row < NUM_PIXELS_Y ; row++){
        copy_row(frame, row , pixels);
    }
}

/**
 * @brief Copies a row of pixel data from the frame to the LED strip.
 * @param frame Pointer to the frame containing the pixel data.
 * @param row Row index of the pixel data to copy.
 * @param pixels Pointer to the 2D array representing the LED strip.
 */
void copy_row(frame_t *frame, uint row , uint32_t **pixels){
    uint col;
    for(col = 0; col < NUM_PIXELS_X ; col++){
        if((row%2)  == LEFT_TO_RIGHT)
            pixels[row][col] = (frame->r[row][col] << 8) | (frame->g[row][col] << 16 ) | frame->b[row][col] ;
        else
            pixels[row][NUM_PIXELS_X - col -1] = (frame->r[row][col] << 8) | (frame->g[row][col] << 16 ) | frame->b[row][col] ;
    }
}

/**
 * @brief Generates random pixel data for each pixel in the frame.
 * @param frame Pointer to the frame to fill with random pixel data.
 */
void randomize(frame_t *frame){
    uint row,col;
    for( row = 0; row < NUM_PIXELS_X; row++)
        for(col = 0; col < NUM_PIXELS_Y; col++){
            frame->r[row][col] = abs(rand()) % 100;
            frame->g[row][col] = abs(rand()) % 100;
            frame->b[row][col] = abs(rand()) % 100;
        }
}

/**
 * @brief Reads pixel data from input data and stores it in the frame.
 * @param frame Pointer to the frame to store the pixel data.
 * @param data Pointer to the input data.
 */
void read_data(frame_t *frame, char * data){
    uint row,col;
    uint8_t pixel_data[3];
    char * pdata = data;
    for( row = 0; row < NUM_PIXELS_X; row++)
        for(col = 0; col < NUM_PIXELS_Y; col++){
            HEADER_PIXEL(pdata,pixel_data);
            frame->r[row][col] = (pixel_data[0] == 0xff)?0x0:pixel_data[0];
            frame->g[row][col] = (pixel_data[1] == 0xff)?0x0:pixel_data[1];
            frame->b[row][col] = (pixel_data[2] == 0xff)?0x0:pixel_data[2];
            printf("pixel values: r=%d,g=%d,b=%d",pixel_data[0],pixel_data[1],pixel_data[2]);
        }
}

/**
 * @brief Writes pixel data to the LED strip using the put_pixel function.
 * @param frame Pointer to the frame containing the pixel data.
 * @param pixels Pointer to the 2D array representing the LED strip.
 */
void write_pixels(frame_t *frame) {
    uint row, col;
    for (row = 0; row < NUM_PIXELS_X; row++) {
        for (col = 0; col < NUM_PIXELS_Y; col++) {
            put_pixel((frame->r[row][col] << 8) | (frame->g[row][col] << 16 ) | frame->b[row][col]);
        }
    }
}

void blink_effect(frame_t *frame, uint32_t **pixels, uint32_t color1, uint32_t color2, int duration) {
    int num_cycles = 1; // Each cycle consists of two states: color1 and color2
    int cycle;
    for (cycle = 0; cycle < num_cycles; cycle++) {
        // Set pixels to color1
        pixel_reset(pixels, color1);
        pack_pixels(frame, pixels);
        write_pixels(frame);
        // Delay for half of the duration
        sleep_ms(duration / 2);

        // Set pixels to color2
        pixel_reset(pixels, color2);
        pack_pixels(frame, pixels);
        write_pixels(frame);
        // Delay for half of the duration
        sleep_ms(duration / 2);
    }
}

void fade_effect(frame_t *frame, uint32_t **pixels, uint32_t color, int duration , uint8_t num_steps) {
    //int num_steps = duration / 10; // Divide duration into 10 steps
    int step;
    uint8_t r, g, b;
    uint8_t target_r, target_g, target_b;
    uint8_t current_r, current_g, current_b;
    uint8_t delta_r, delta_g, delta_b;

    // Get the current color values
    current_r = (color >> 8) & 0xFF;
    current_g = (color >> 16) & 0xFF;
    current_b = color & 0xFF;

    // Calculate the target color values
    target_r = current_r / 2;
    target_g = current_g / 2;
    target_b = current_b / 2;

    // Calculate the delta values for each color component
    delta_r = (target_r - current_r) >> num_steps;
    delta_g = (target_g - current_g) >> num_steps;
    delta_b = (target_b - current_b) >> num_steps;

    // Perform the fading effect
    for (step = 0; step < 2<<num_steps; step++) {
        // Update the color values
        current_r += delta_r;
        current_g += delta_g;
        current_b += delta_b;

        // Set pixels to the updated color
        pixel_reset(pixels, (current_r << 8) | (current_g << 16) | current_b);
        pack_pixels(frame, pixels);
        write_pixels(frame);

        // Delay for 10 milliseconds
        sleep_ms(10);
    }
}

void randomized_effect(frame_t *frame, uint32_t **pixels, int duration) {
    int num_cycles = 10; // Each cycle consists of two states: random colors and black
    int cycle;
    for (cycle = 0; cycle < num_cycles; cycle++) {
        // Generate random colors for each pixel
        randomize(frame);
        pack_pixels(frame, pixels);
        write_pixels(frame);
        // Delay for half of the duration
        sleep_ms(duration / 2);
        // Set pixels to black
        pixel_reset(pixels, 0x000000);
        pack_pixels(frame, pixels);
        write_pixels(frame);
        // Delay for half of the duration
        sleep_ms(duration / 2);
    }
}

void solid_color_effect(frame_t *frame, uint32_t **pixels, uint32_t color) {
    // Set all pixels to the specified color
    pixel_reset(pixels, color);
    pack_pixels(frame, pixels);
    write_pixels(frame);
}

void clear_effect(frame_t *frame, uint32_t **pixels) {
    // Set all pixels to black
    pixel_reset(pixels, 0x000000);
    pack_pixels(frame, pixels);
    write_pixels(frame);
}

void running_light_effect(frame_t *frame, uint32_t **pixels, uint32_t color, int duration) {
    int num_pixels = NUM_PIXELS_X * NUM_PIXELS_Y;
    int num_cycles = duration / num_pixels;
    int cycle;
    for (cycle = 0; cycle < num_cycles; cycle++) {
        int i;
        for (i = 0; i < num_pixels; i++) {
            // Set the current pixel to the specified color
            int row = i / NUM_PIXELS_X;
            int col = i % NUM_PIXELS_X;
            pixels[row][col] = color;
            pack_pixels(frame, pixels);
            write_pixels(frame);
            // Delay for a short duration
            sleep_ms(duration / num_pixels);
            // Set the current pixel back to black
            pixels[row][col] = 0x000000;
            pack_pixels(frame, pixels);
            write_pixels(frame);
            sleep_ms(100);
        }
    }
}

void fade_in_out_effect(frame_t *frame, uint32_t **pixels, uint32_t color,uint8_t fading_target,uint8_t num_steps, int duration) {
    //int num_steps = duration / 10; // Divide duration into 10 steps
    int step;
    uint8_t r, g, b;
    uint8_t target_r, target_g, target_b;
    uint8_t current_r, current_g, current_b;
    uint8_t delta_r, delta_g, delta_b;

    // Get the current color values
    current_r = (color >> 8) & 0xFF;
    current_g = (color >> 16) & 0xFF;
    current_b = color & 0xFF;

    // Calculate the target color values
    target_r = current_r >> fading_target;
    target_g = current_g >> fading_target;
    target_b = current_b >> fading_target;

    // Calculate the delta values for each color component
    delta_r = (target_r - current_r) >> num_steps;
    delta_g = (target_g - current_g) >> num_steps;
    delta_b = (target_b - current_b) >> num_steps;

    // Perform the fading effect
    for (step = 0; step < 2<<num_steps; step++) {
        // Update the color values
        current_r += delta_r;
        current_g += delta_g;
        current_b += delta_b;

        // Set pixels to the updated color
        pixel_reset(pixels, (current_r << 8) | (current_g << 16) | current_b);
        pack_pixels(frame, pixels);
        write_pixels(frame);

        // Delay for 10 milliseconds
        sleep_ms(10);
    }

    // Perform the fading effect in reverse
    for (step = 0; step < 2<<num_steps; step++) {
        // Update the color values
        current_r -= delta_r;
        current_g -= delta_g;
        current_b -= delta_b;

        // Set pixels to the updated color
        pixel_reset(pixels, (current_r << 8) | (current_g << 16) | current_b);
        pack_pixels(frame, pixels);
        write_pixels(frame);

        // Delay for 10 milliseconds
        sleep_ms(10);
    }
}