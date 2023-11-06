#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812.h"
#include "ws2812.pio.h"


#define NUM_BUFFER 2
// bit plane content dma channel
#define DMA_CHANNEL 0
// chain channel for configuring main dma channel to output from disjoint 8 word fragments of memory
#define DMA_CB_CHANNEL 1

#define DMA_CHANNEL_MASK (1u << DMA_CHANNEL)
#define DMA_CB_CHANNEL_MASK (1u << DMA_CB_CHANNEL)
#define DMA_CHANNELS_MASK (DMA_CHANNEL_MASK | DMA_CB_CHANNEL_MASK)

// start of each value fragment (+1 for NULL terminator)
static uint32_t fragment_start[TOTAL_PIXELS + 1];

pixel_line_t screen[NUM_BUFFER][NUM_PIXELS_Y];

frame_t image[NUM_BUFFER];


// posted when it is safe to output a new set of values
static struct semaphore reset_delay_complete_sem;
// alarm handle for handling delay
alarm_id_t reset_delay_alarm_id;

int64_t reset_delay_complete(alarm_id_t id, void *user_data) {
    reset_delay_alarm_id = 0;
    sem_release(&reset_delay_complete_sem);
    // no repeat
    return 0;
}

void __isr dma_complete_handler() {
    if (dma_hw->ints0 & DMA_CHANNEL_MASK) {
        // clear IRQ
        dma_hw->ints0 = DMA_CHANNEL_MASK;
        // when the dma is complete we start the reset delay timer
        if (reset_delay_alarm_id) cancel_alarm(reset_delay_alarm_id);
        reset_delay_alarm_id = add_alarm_in_us(400, reset_delay_complete, NULL, true);
    }
}

void dma_init(PIO pio, uint sm) {
    dma_claim_mask(DMA_CHANNELS_MASK);

    // main DMA channel outputs 8 word fragments, and then chains back to the chain channel
    dma_channel_config channel_config = dma_channel_get_default_config(DMA_CHANNEL);
    channel_config_set_dreq(&channel_config, pio_get_dreq(pio, sm, true));
    channel_config_set_chain_to(&channel_config, DMA_CB_CHANNEL);
    channel_config_set_irq_quiet(&channel_config, true);
    dma_channel_configure(DMA_CHANNEL,
                          &channel_config,
                          &pio->txf[sm],
                          NULL, // set by chain
                          8, // 8 words for 8 bit planes
                          false);

    // chain channel sends single word pointer to start of fragment each time
    dma_channel_config chain_config = dma_channel_get_default_config(DMA_CB_CHANNEL);
    dma_channel_configure(DMA_CB_CHANNEL,
                          &chain_config,
                          &dma_channel_hw_addr(
                                  DMA_CHANNEL)->al3_read_addr_trig,  // ch DMA config (target "ring" buffer size 4) - this is (read_addr trigger)
                          NULL, // set later
                          1,
                          false);

    irq_set_exclusive_handler(DMA_IRQ_0, dma_complete_handler);
    dma_channel_set_irq0_enabled(DMA_CHANNEL, true);
    irq_set_enabled(DMA_IRQ_0, true);
}

void output_strips_dma(pixel_line_t *bits, uint value_length) {

    uint row = 0; 
    uint col = 0;

    for (uint i = 0; i < value_length; i++,col++) {
        if(col == NUM_PIXELS_X){
            col = 0;
            bits = bits->next_ptr;
        }
        fragment_start[i] =  bits->data[col]; // MSB first
    }
    fragment_start[value_length] = 0;
    dma_channel_hw_addr(DMA_CB_CHANNEL)->al3_read_addr_trig = (uintptr_t) fragment_start;
}

int main() {
    //set_sys_clock_48();
    stdio_init_all();
    puts("WS2812 parallel");

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    bool current = 0;

    pixel_chain_init(screen[0] , LEFT_TO_RIGHT , TOTAL_PIXELS);
    pixel_chain_init(screen[1] , LEFT_TO_RIGHT , TOTAL_PIXELS);

    ws2812_program_init(pio, sm, offset, WS2812_PIN_BASE, 1, 800000);

    sem_init(&reset_delay_complete_sem, 1, 1); // initially posted so we don't block first time
    dma_init(pio, sm);
    int t = 0;
    while (1) {

        randomize(&image[current]);
        pack_pixels(&image[current] , screen[current]);

        int brightness = 0;
        uint current = 0;
        for (int i = 0; i < 1000; ++i) {

            sem_acquire_blocking(&reset_delay_complete_sem);
            output_strips_dma(screen[current], TOTAL_PIXELS * 4);

            current ^= 1;

        }
    }
}

