#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "matrix_display.hpp"
#include "pindefs.hpp"
#include "clw_dbgutils.h"
#include "pico/multicore.h"

#include "default.hpp"
#include "tetris.hpp"

void init_gpio(void){
    gpio_init_mask(MASK_ALL_COLS|MASK_ALL_ROWS);
    gpio_set_dir_masked(MASK_ALL_COLS|MASK_ALL_ROWS, MASK_ALL_COLS|MASK_ALL_ROWS);
    gpio_put_masked(MASK_ALL_ROWS,MASK_ALL_ROWS);
    gpio_set_drive_strength(LED_C1, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C2, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C3, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C4, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C5, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_dir(PB1,0);
    gpio_set_dir(PB2,0);
    gpio_pull_up(PB1);
    gpio_pull_up(PB2);
}

struct behaviour current_behaviour = default_behav;

// Tick interrupt
repeating_timer_t scroll_timer = {0};
bool scroll_timer_cb(repeating_timer_t * timer){
    current_behaviour.Tick();
    return true;
}

void print_info(void){
    printf("------------------------------------------------\n");
    printf(BR_BLUE "ECSE LEAVERS DINNER INVITATIONS 2025\n" COLOUR_NONE);
    printf("------------------------------------------------\n");
    printf("Hello! Congratulations on Finishing your undergraduate degree.\n");
    printf("To update the displayed string, simply send a new one over serial.\n");
    printf("All ASCII printable characters should be implemented.\n");
    printf("Try the buttons to see if you can find any easter eggs,\n");
    printf("or check the git repo for more info.\n");
    printf("github.com/campbelllwright/ECSE_LeaversDinner_2025\n");
    printf("--------------------------------------------------\n");
    printf(" - PCB by : Campbell Wright, James West\n");
    printf(" - Code by: Campbell Wright, Mekal Covic\n");
    printf("---------------------------------------\n");
}

// debouncing
bool Smooth(const bool raw, bool * const arr18)
{
#define idx ((unsigned char*)arr18)[16]
#define val (arr18[17])
    arr18[idx] = raw;
    uint8_t notVal = !val;

    // probably saves a nanosecond over a loop
    if (
        arr18[0] == notVal && arr18[1] == notVal && arr18[2] == notVal &&
        arr18[3] == notVal && arr18[4] == notVal && arr18[5] == notVal &&
        arr18[6] == notVal && arr18[7] == notVal && arr18[8] == notVal &&
        arr18[9] == notVal && arr18[10] == notVal && arr18[11] == notVal &&
        arr18[12] == notVal && arr18[13] == notVal && arr18[14] == notVal &&
        arr18[15] == notVal
    ) {
        val = notVal;
    }

    // increment
    idx++;
    idx &= 0xF;

    return val;
}
#undef idx
#undef val

// Core 1: Poll GPIO inputs
void Poll(void) {
    static bool pb1_arr[18] = { true }; // nb this only sets [0] to true
    pb1_arr[17] = true;
    static bool pb2_arr[18] = { true };
    pb1_arr[17] = true;
    
    for (;;) {
        static bool pb1_last = 1, pb2_last = 1;
        
        bool pb1_val = Smooth(gpio_get(PB1), pb1_arr);
        bool pb2_val = Smooth(gpio_get(PB2), pb2_arr);

        // on edge, change
        if((pb1_last != pb1_val)||(pb2_last!=pb2_val)){
            if((pb1_val==0) &&(pb2_val ==0)){
                multicore_fifo_push_blocking(I_PB1_PB2);
            } else if (!pb1_val) {
                multicore_fifo_push_blocking(I_PB1);
            } else if (!pb2_val) {
                multicore_fifo_push_blocking(I_PB2);
            }
        }

        pb1_last = pb1_val;
        pb2_last = pb2_val;
    }
}

static char tempBuffer[128] = {0};
static uint8_t tempBufferIdx = 0;

int main()
{
    tempBuffer[0] = ' ';
    tempBufferIdx = 1;

    stdio_init_all();
    init_gpio();
    DefaultBehaviourInit();
    InitTetris();
    printf("hello, world!");
    add_repeating_timer_ms(-100,scroll_timer_cb,0,&scroll_timer);
    multicore_launch_core1(Poll);
    
    while (true) {
        uint32_t r;
        
        if (multicore_fifo_pop_timeout_us(10, &r)) {
            current_behaviour.Input( (gpio_input)r );
        }

        current_behaviour.PreRender();

        for(int i = 0; i < 100; i++)
        {
            current_behaviour.DrawFrame();    
            //This is janky - ISRs were being weird so we just do 100 display cycles for every button poll
            //which means our polling rate is worst case 100us*25*100 = 250ms.
            //if ISRs still funky maybe throw this on core 1? would be cool and leave core 0 available for user code/polling.
        }

        current_behaviour.PostRender();

        // Debug stuff
        char inChar = getchar_timeout_us(10);
        
        if(inChar != 0xFE){
            if(inChar != '\n'){
                tempBuffer[tempBufferIdx] = inChar;
                tempBufferIdx++;
            }
            else{
                tempBuffer[tempBufferIdx] = 0;
                DefaultBehaviourDebug(tempBuffer);
                tempBuffer[0] = ' ';
                tempBufferIdx = 1;
            }
        }
        
    }
}
