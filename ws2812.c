/**
 * @file ws2812.c
 * @brief Implementation of functions for controlling WS2812 LED strip.
 */

#include "ws2812.h"

pixel_t pixels;

/**
 * @brief Extracts the red component from a 32-bit color value.
 * @param color The 32-bit color value.
 * @return The red component value.
 */
uint8_t extract_r(uint32_t color) {
    return (color >> 16) & 0xFF;
}

/**
 * @brief Extracts the green component from a 32-bit color value.
 * @param color The 32-bit color value.
 * @return The green component value.
 */
uint8_t extract_g(uint32_t color) {
    return (color >> 8) & 0xFF;
}

/**
 * @brief Extracts the blue component from a 32-bit color value.
 * @param color The 32-bit color value.
 * @return The blue component value.
 */
uint8_t extract_b(uint32_t color) {
    return color & 0xFF;
}

/**
 * @brief Sets the RGB values of a pixel.
 * @param pixel Pointer to the pixel.
 * @param color The 32-bit color value.
 */
void set_pixel(pixel_t *pixel, uint32_t color, uint8_t brightness) {
    pixel->r = extract_r(color)*brightness/100;
    pixel->g = extract_g(color)*brightness/100;
    pixel->b = extract_b(color)*brightness/100;

}

/**
 * @brief Sets the RGB values of a segment of pixels.
 * @param segment Pointer to the segment.
 * @param dir The orientation of the segment.
 * @param color Array of 32-bit color values.
 * @param num_pixels Number of pixels in the segment.
 */
void set_segment(segment_t *segment, orientation_t dir, uint32_t *color,
                 uint8_t num_pixels, uint8_t brightness) {
     if (num_pixels == 1) {
        for (int i = 0; i < PIXELS_PER_SEG ; i++)
            set_pixel(&(segment->pixels[i]), *color,brightness);
    }else if(num_pixels < PIXELS_PER_SEG){
        for (int i = 0; i < num_pixels; i++)
            set_pixel(&(segment->pixels[i]), color[i],brightness);
    }
    else {
        if(num_pixels > PIXELS_PER_SEG)
            num_pixels = PIXELS_PER_SEG;

        for (int i = 0; i < num_pixels; i++) {
            if (dir == ORIENTATION_UP) {
                set_pixel(&(segment->pixels[i]), color[i % num_pixels],brightness);
            } else {
                set_pixel(&(segment->pixels[i]), color[num_pixels - 1 - i % num_pixels],brightness);
            }
        }
    }
}

/**
 * @brief Initializes the WS2812 LED strip.
 * @param ws2812 Pointer to the ws2812_t structure.
 */
void ws2812_init(ws2812_t *ws2812, PIO pio, uint state_machine_id, uint8_t pin_base, uint freq){


    ws2812->pio = pio;
    ws2812->state_machine_id= pio_claim_unused_sm(pio, true);
    ws2812->pin_base = pin_base;
    ws2812->freq = freq;

    uint offset = pio_add_program(pio, &ws2812_program);




    ws2812_program_init(ws2812->pio, ws2812->state_machine_id, offset, ws2812->pin_base, ws2812->freq, false);

    for (int i = 0; i < NUM_SEG; i++) {
        ws2812->orientation[i] = ORIENTATION_UP;
        for (int j = 0; j < PIXELS_PER_SEG; j++) {
            ws2812->segments[i].pixels[j].r = 0;
            ws2812->segments[i].pixels[j].g = 0;
            ws2812->segments[i].pixels[j].b = 0;
        }
    }
}

/**
 * @brief Sends the pixel data to the WS2812 LED strip.
 * @param ws2812 Pointer to the ws2812_t structure.
 */
void ws2812_show(ws2812_t *ws2812) {
    for (int i = 0; i < NUM_SEG; i++) {
        for (int j = 0; j < PIXELS_PER_SEG; j++) {
            pio_sm_put_blocking(ws2812->pio, ws2812->state_machine_id, (ws2812->segments[i].pixels[j].g << 16 | ws2812->segments[i].pixels[j].r << 8 | ws2812->segments[i].pixels[j].b) << 8u);
        }
    }
}

/**
 * @brief Clears all the pixels on the WS2812 LED strip.
 * @param ws2812 Pointer to the ws2812_t structure.
 */
void ws2812_clear(ws2812_t *ws2812) {
    uint32_t color = 0x000000;
    for (int i = 0; i < NUM_SEG; i++) {
        set_segment(&(ws2812->segments[i]), ws2812->orientation[i], &color, 1 , 0x0);
    }
    ws2812_show(ws2812);
}

/**
 * @brief Sets the color of a specific pixel on the WS2812 LED strip.
 * @param ws2812 Pointer to the ws2812_t structure.
 * @param seg The segment index.
 * @param pixel The pixel index.
 * @param color The 32-bit color value.
 */
void ws2812_set_pixel(ws2812_t *ws2812, uint8_t seg, uint8_t pixel, uint32_t color,
                        uint8_t brightness) {
    if (seg < NUM_SEG && pixel < PIXELS_PER_SEG) {
        set_pixel(&ws2812->segments[seg].pixels[pixel], color,brightness );
    }
}

/**
 * @brief Sets the color of a segment of pixels on the WS2812 LED strip.
 * @param ws2812 Pointer to the ws2812_t structure.
 * @param seg The segment index.
 * @param dir The orientation of the segment.
 * @param color Array of 32-bit color values.
 * @param num_pixels Number of pixels in the segment.
 */
void ws2812_set_segment(ws2812_t *ws2812, uint8_t seg, orientation_t dir,
                         uint32_t *color, uint8_t num_pixels, uint8_t brightness) {
    if (seg < NUM_SEG) {
        ws2812->orientation[seg] = dir;
        set_segment(&(ws2812->segments[seg]), dir, color, num_pixels,brightness);
    }
}

/**
 * @brief Fades the color of the WS2812 LED strip from one color to another.
 * @param ws2812 Pointer to the ws2812_t structure.
 * @param color1 The starting color.
 * @param color2 The ending color.
 * @param delay_ms The delay between each color change.
 */
void ws2812_fade(ws2812_t *ws2812, uint32_t color1, uint32_t color2,
             uint32_t delay_ms, uint8_t brightness) {
    int r1 = extract_r(color1);
    int g1 = extract_g(color1);
    int b1 = extract_b(color1);
    int r2 = extract_r(color2);
    int g2 = extract_g(color2);
    int b2 = extract_b(color2);
    int r_diff = r2 - r1;
    int g_diff = g2 - g1;
    int b_diff = b2 - b1;
    int r_step = r_diff > 0 ? 1 : -1;
    int g_step = g_diff > 0 ? 1 : -1;
    int b_step = b_diff > 0 ? 1 : -1;
    int r = r1;
    int g = g1;
    int b = b1;
    while (r != r2 || g != g2 || b != b2) {
        uint32_t color = (g << 16) | (r << 8) | b;
        for (int i = 0; i < NUM_SEG; i++) {
            ws2812_set_segment(ws2812, i, ORIENTATION_UP, &color, 1,brightness);
        }
        ws2812_show(ws2812);
        sleep_ms(delay_ms);
        if (r != r2) r += r_step;
        if (g != g2) g += g_step;
        if (b != b2) b += b_step;
    }
}

/**
 * @brief Sets all the pixels on the WS2812 LED strip to a solid color.
 * @param ws2812 Pointer to the ws2812_t structure.
 * @param color The 32-bit color value.
 */
void ws2812_solid_color(ws2812_t *ws2812, uint32_t color,uint8_t brightness) {
    for (int i = 0; i < NUM_SEG; i++) {
        ws2812_set_segment(ws2812, i, ORIENTATION_UP, &color, 1,brightness);
    }
    ws2812_show(ws2812);
}

/**
 * @brief Blinks the WS2812 LED strip between two colors.
 * @param ws2812 Pointer to the ws2812_t structure.
 * @param color1 The first color.
 * @param color2 The second color.
 * @param delay_ms The delay between each color change.
 */
void ws2812_blink(ws2812_t *ws2812, uint32_t color1, uint32_t color2,
         uint32_t delay_ms, uint8_t brightness) {
    ws2812_solid_color(ws2812, color1, brightness);
    sleep_ms(delay_ms);
    ws2812_solid_color(ws2812, color2, brightness);
    sleep_ms(delay_ms);
}

/**
 * @brief Runs a light along the WS2812 LED strip.
 * @param ws2812 Pointer to the ws2812_t structure.
 * @param color The color of the light.
 * @param delay_ms The delay between each movement of the light.
 */
void ws2812_running_light(ws2812_t *ws2812, uint32_t color,
             uint32_t delay_ms, uint8_t brightness) {
    for (int i = 0; i < PIXELS_PER_SEG; i++) {
        for (int j = 0; j < NUM_SEG; j++) {
            ws2812_set_pixel(ws2812, j, i, color,brightness);
        }
        ws2812_show(ws2812);
        sleep_ms(delay_ms);
    }
    for (int i = PIXELS_PER_SEG - 1; i >= 0; i--) {
        for (int j = 0; j < NUM_SEG; j++) {
            ws2812_set_pixel(ws2812, j, i, 0x000000,brightness);
        }
        ws2812_show(ws2812);
        sleep_ms(delay_ms);
    }
}

/**
 * @brief Fades the WS2812 LED strip in and out with a specific color.
 * @param ws2812 Pointer to the ws2812_t structure.
 * @param color The color to fade in and out.
 * @param delay_ms The delay between each fade step.
 */
void ws2812_fade_in_out(ws2812_t *ws2812, uint32_t color,
                         uint32_t delay_ms,uint8_t brightness) {
    ws2812_fade(ws2812, 0x000000, color, delay_ms,brightness);
    ws2812_fade(ws2812, color, 0x000000, delay_ms,brightness);
}