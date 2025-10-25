#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "matrix_display.hpp"
#include "pindefs.hpp"
#include "pico_flash.hpp"

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
const uint8_t * current_char;
//const char * testString = ;

char stringBuffer[STR_BUFFER_LEN] = "Campbell Wright ";
char tempBuffer[STR_BUFFER_LEN] = {0};
uint8_t tempBufferIdx = 0;
int main()
{
    stdio_init_all();
    init_gpio();
    read_name_from_flash(stringBuffer, STR_BUFFER_LEN);
    printf("hello, world!");
    
    while (true) {
        static bool pb1_last = 1;
        bool pb1_val = gpio_get(PB1);
        if((pb1_val == 0)&&(pb1_last != pb1_val)){
            counter = (counter+1) % strlen(stringBuffer);
        }
        pb1_last = pb1_val;
        if(gpio_get(PB2)){
            current_char=char_to_matrix(stringBuffer[counter]);
        }
        else{
            current_char = char_to_matrix(128);
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
                memccpy(stringBuffer, tempBuffer, 0, 64);
                uint rc = write_name_to_flash(stringBuffer);
                if(!rc){
                    printf("wrote string \"%s\" (%d bytes) to flash\n", stringBuffer, strlen(stringBuffer)+1);
                }
                counter = 0;
                tempBufferIdx = 0;
            }
        }
        
    }
}
