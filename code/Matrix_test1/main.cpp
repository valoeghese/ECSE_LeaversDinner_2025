#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "matrix_display.hpp"
#include "pindefs.hpp"
#include "pico_flash.hpp"
#include "clw_dbgutils.h"
#include "pico/multicore.h"

#define STR_BUFFER_LEN 128

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

uint counter = 0;
uint scroll_count = 0;
const uint8_t * current_char;
//const char * testString = ;
enum disp_mode{
    USER = 0,
    ECSE = 1,
    EASTER = 2,
    TETRIS
};

disp_mode display_mode = USER;


char userStringBuffer[STR_BUFFER_LEN] = " Use PuTTY to Program (115200b)";
char presetStringBuffer[STR_BUFFER_LEN] = " ECSE LEAVERS 2025";
char easterEggStr[STR_BUFFER_LEN] = " COMPSYS ON TOP";
char tempBuffer[STR_BUFFER_LEN] = {0};

char * dispBuff = userStringBuffer;

char * strings[] = {
    userStringBuffer,
    presetStringBuffer,
    easterEggStr
};
uint8_t tempBufferIdx = 0;
void scroll_screen(void){
    if (display_mode < TETRIS) {
        scroll_chars();
        scroll_count++;
        if(scroll_count==7){
            counter = (counter+1) % strlen(strings[display_mode]);
            const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
            add_char_to_scroll(disp_char);
            scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
        }
    }
}

void screen_start(void){
    const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
    add_char_to_scroll(disp_char);
    scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
}

repeating_timer_t scroll_timer = {0};
bool scroll_timer_cb(repeating_timer_t * timer){
    scroll_screen();
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
        arr18[0] == notVal &&
        arr18[1] == notVal &&
        arr18[2] == notVal &&
        arr18[3] == notVal &&
        arr18[4] == notVal &&
        arr18[5] == notVal &&
        arr18[6] == notVal &&
        arr18[7] == notVal &&
        arr18[8] == notVal &&
        arr18[9] == notVal &&
        arr18[10] == notVal &&
        arr18[11] == notVal &&
        arr18[12] == notVal &&
        arr18[13] == notVal &&
        arr18[14] == notVal &&
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
                multicore_fifo_push_blocking(EASTER);
            } else if (!pb1_val) {
                multicore_fifo_push_blocking(TETRIS);
            } else if (!pb2_val) {
                static bool toggle = 0;

                multicore_fifo_push_blocking((toggle ^= 1) ? USER : ECSE);
            }
        }

        pb1_last = pb1_val;
        pb2_last = pb2_val;
    }
}

int main()
{
    tempBuffer[0] = ' ';
    tempBufferIdx = 1;
    stdio_init_all();
    init_gpio();
    read_name_from_flash(userStringBuffer, STR_BUFFER_LEN);
    screen_start();
    printf("hello, world!");
    add_repeating_timer_ms(-100,scroll_timer_cb,0,&scroll_timer);
    multicore_launch_core1(Poll);
    
    while (true) {
        uint32_t r;
        
        if (multicore_fifo_pop_timeout_us(10, &r)) {
            display_mode = (disp_mode)r;

            //3MSB of first col of char = length (0-7)
            counter = 0;
            if (display_mode < TETRIS) {
                const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
                add_char_to_scroll_start(disp_char);
                add_char_to_scroll(disp_char);
                scroll_count=5-((disp_char[0]&0xE0)>>5);
            }
        }

        // Clear when holding down pb
        if(gpio_get(PB2)){
            current_char=scroll_buff;//char_to_matrix(stringBuffer[counter]);
        }

        for(int i = 0; i < 100; i++){
            disp_char(current_char); 
            //This is janky - ISRs were being weird so we just do 100 display cycles for every button poll
            //which means our polling rate is worst case 100us*25*100 = 250ms.
            //if ISRs still funky maybe throw this on core 1? would be cool and leave core 0 available for user code/polling.
        }

        char inChar = getchar_timeout_us(10);
        if(inChar != 0xFE){
            if(inChar != '\n'){
                tempBuffer[tempBufferIdx] = inChar;
                tempBufferIdx++;
            }
            else{
                tempBuffer[tempBufferIdx] = 0;
                printf("Displaying String \"%s\"\n", tempBuffer);
                memccpy(userStringBuffer, tempBuffer, 0, 64);
                uint rc = write_name_to_flash(userStringBuffer);
                if(!rc){
                    printf("wrote string \"%s\" (%d bytes) to flash\n", userStringBuffer, strlen(userStringBuffer)+1);
                }
                counter = 0;
                tempBuffer[0] = ' ';
                tempBufferIdx = 1;
            }
        }
        
    }
}
