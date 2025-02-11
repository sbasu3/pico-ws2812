#include <stdio.h>
#include <assert.h>
#include "ws2812.h"

int test(ws2812_t ws2812) {
    // Test extract_r function
    uint32_t color = 0xFF00FF; // RGB: 255, 0, 255
    uint8_t r = extract_r(color);
    assert(r == 255);

    // Test extract_g function
    uint8_t g = extract_g(color);
    assert(g == 0);

    // Test extract_b function
    uint8_t b = extract_b(color);
    assert(b == 255);

    // Test set_pixel function
    pixel_t pixel;
    uint8_t brightness = 50;
    set_pixel(&pixel, color,brightness);
    assert(pixel.r == 255*brightness/100);
    assert(pixel.g == 0);
    assert(pixel.b == 255*brightness/100);

    // Test set_segment function
    segment_t segment;
    uint32_t colors[3] = {0xFF0000, 0x00FF00, 0x0000FF}; // RGB: red, green, blue
    set_segment(&segment, ORIENTATION_UP, colors, 3,50);
    assert(segment.pixels[0].r == 255*brightness/100);
    assert(segment.pixels[0].g == 0);
    assert(segment.pixels[0].b == 0);
    assert(segment.pixels[1].r == 0);
    assert(segment.pixels[1].g == 255*brightness/100);
    assert(segment.pixels[1].b == 0);
    assert(segment.pixels[2].r == 0);
    assert(segment.pixels[2].g == 0);
    assert(segment.pixels[2].b == 255*brightness/100);

    // Test ws2812_init function
    //ws2812_t ws2812;
    ws2812_init(&ws2812 , ws2812.pio , ws2812.state_machine_id,ws2812.pin_base,ws2812.freq);
    for (int i = 0; i < NUM_SEG; i++) {
        assert(ws2812.orientation[i] == ORIENTATION_UP);
        for (int j = 0; j < PIXELS_PER_SEG; j++) {
            assert(ws2812.segments[i].pixels[j].r == 0);
            assert(ws2812.segments[i].pixels[j].g == 0);
            assert(ws2812.segments[i].pixels[j].b == 0);
        }
    }

    // Test ws2812_clear function
    ws2812_clear(&ws2812);
    for (int i = 0; i < NUM_SEG; i++) {
        for (int j = 0; j < PIXELS_PER_SEG; j++) {
            assert(ws2812.segments[i].pixels[j].r == 0);
            assert(ws2812.segments[i].pixels[j].g == 0);
            assert(ws2812.segments[i].pixels[j].b == 0);
        }
    }

    // Test ws2812_set_pixel function
    ws2812_set_pixel(&ws2812, 0, 0, 0xFF00FF,50); // Set first pixel of first segment to RGB: 255, 0, 255
        assert(ws2812.segments[0].pixels[0].r == 255*brightness/100);
        assert(ws2812.segments[0].pixels[0].g == 0);
        assert(ws2812.segments[0].pixels[0].b == 255*brightness/100);

    // Test ws2812_set_segment function
    uint32_t segment_colors[2] = {0xFFFF00, 0x00FFFF}; // RGB: yellow, cyan
    ws2812_set_segment(&ws2812, 1, ORIENTATION_DOWN, segment_colors, 2,50);
    assert(ws2812.segments[1].pixels[0].r == 255*brightness/100);
    assert(ws2812.segments[1].pixels[0].g == 255*brightness/100);
    assert(ws2812.segments[1].pixels[0].b == 0);
    assert(ws2812.segments[1].pixels[1].r == 0);
    assert(ws2812.segments[1].pixels[1].g == 255*brightness/100);
    assert(ws2812.segments[1].pixels[1].b == 255*brightness/100);


    ws2812_clear(&ws2812);
    // Test ws2812_fade function
    ws2812_fade(&ws2812, 0xFF0000, 0x00FF00, 40,50); // Fade from red to green with 100ms delay
    // Manual verification required
    ws2812_clear(&ws2812);
    // Test ws2812_solid_color function
    ws2812_solid_color(&ws2812, 0x0000FF,50); // Set all pixels to blue
    // Manual verification required
    ws2812_clear(&ws2812);
    // Test ws2812_blink function
    ws2812_blink(&ws2812, 0xFF0000, 0x00FF00, 500,50); // Blink between red and green with 500ms delay
    // Manual verification required
    ws2812_clear(&ws2812);
    // Test ws2812_running_light function
    ws2812_running_light(&ws2812, 0xFFFF00, 200,50); // Run a yellow light with 200ms delay
    // Manual verification required
    ws2812_clear(&ws2812);
    // Test ws2812_fade_in_out function
    ws2812_fade_in_out(&ws2812, 0x00FFFF, 40,50); // Fade in and out with cyan color and 100ms delay
    // Manual verification required
    ws2812_clear(&ws2812);
    printf("All tests passed!\n");

    return 0;
}
