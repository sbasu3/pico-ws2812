#include "ws2812.h"

pixel_line_t pixels[NUM_PIXELS_Y];




void pixel_chain_init(pixel_line_t *pixels , bool dir , uint16_t num_pixels){

    int i = 0;
    
    uint sizeY = num_pixels/NUM_PIXELS_X;

    pixels 

    for(i = 0; i < sizeY; i++){
        
        pixels->dir = dir;
        dir = ~dir;
        pixels->next_ptr = malloc(sizeof(pixel_line_t));
        pixels = pixels->next_ptr;
    } 
    pixels->next_ptr = NULL;
}


void pack_pixels(frame_t *frame , pixel_line_t *pixels){

    uint row,col;

    for( col = 0; col < NUM_PIXELS_Y ; col++){
        copy_row(frame, col*NUM_PIXELS_X , pixels);
        pixels = pixels->next_ptr;
    }
}

void copy_row(frame_t *frame, uint start_addr, pixel_line_t *pixels){

    uint col = 0;
    uint row = start_addr/NUM_PIXELS_X;

    for(col = 0; col < NUM_PIXELS_X ; col++){
        if(pixels->dir == LEFT_TO_RIGHT)
            pixels->data[col] = (frame->r[row][col] << 8) | (frame->g[row][col] << 16 ) | frame->b[row][col] ;
        else
            pixels->data[NUM_PIXELS_X - col -1] = (frame->r[row][col] << 8) | (frame->g[row][col] << 16 ) | frame->b[row][col] ;

    }
}

void randomize(frame_t *frame){

    uint row,col;

    for( row = 0; row < NUM_PIXELS_X; row++)
        for(col = 0; col < NUM_PIXELS_Y; col++){
            frame->r[row][col] = rand() % 100;
            frame->g[row][col] = rand() % 100;
            frame->b[row][col] = rand() % 100;
        }

}