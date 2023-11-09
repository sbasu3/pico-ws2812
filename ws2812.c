#include "ws2812.h"

static unsigned int width = 8;
static unsigned int height = 8;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data,pixel) {\
pixel[2] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[0] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}


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