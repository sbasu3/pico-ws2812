#include "ws2812.h"

void pixel_reset(uint32_t **pixels , uint32_t val){

    uint row,col;

    for(row = 0 ; row < NUM_PIXELS_X ; row++)
        for(col = 0; col < NUM_PIXELS_Y ; col++)
            pixels[row][col] = val;
}


void pack_pixels(frame_t *frame ,uint32_t **pixels){

    uint row,col;

    for( row = 0; row < NUM_PIXELS_Y ; row++){
        copy_row(frame, row , pixels);
    }
}

void copy_row(frame_t *frame, uint row , uint32_t **pixels){

    uint col;

    for(col = 0; col < NUM_PIXELS_X ; col++){
        if((row%2)  == LEFT_TO_RIGHT)
            pixels[row][col] = (frame->r[row][col] << 8) | (frame->g[row][col] << 16 ) | frame->b[row][col] ;
        else
            pixels[row][NUM_PIXELS_X - col -1] = (frame->r[row][col] << 8) | (frame->g[row][col] << 16 ) | frame->b[row][col] ;

    }
}

void randomize(frame_t *frame){

    uint row,col;

    for( row = 0; row < NUM_PIXELS_X; row++)
        for(col = 0; col < NUM_PIXELS_Y; col++){
            frame->r[row][col] = abs(rand()) % 100;
            frame->g[row][col] = abs(rand()) % 100;
            frame->b[row][col] = abs(rand()) % 100;
        }

}