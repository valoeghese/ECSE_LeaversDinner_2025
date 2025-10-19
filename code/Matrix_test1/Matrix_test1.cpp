#include <stdio.h>
#include "pico/stdlib.h"

// LED MATRIX COLUMNS ARE POSITIVE, ROWS ARE NEGATIVE
#define LED_C1 25
#define LED_C2 24
#define LED_C3 23
#define LED_C4 22
#define LED_C5 21

#define MASK_ALL_COLS ((1<<LED_C1)|(1<<LED_C2)|(1<<LED_C3)|(1<<LED_C4)|(1<<LED_C5))

#define LED_R1 16
#define LED_R2 17
#define LED_R3 18
#define LED_R4 19
#define LED_R5 20

#define MASK_ALL_ROWS ((1<<LED_R1)|(1<<LED_R2)|(1<<LED_R3)|(1<<LED_R4)|(1<<LED_R5))

#define PB1 15
#define PB2 14

int main()
{
    stdio_init_all();

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
    
    while (true) {
        //printf("Hello, world!\n");
        if(gpio_get(PB1)){
            gpio_set_dir_masked((1<<LED_R1)|(1<<LED_R2), (1<<LED_R2));
            gpio_put_masked((1<<LED_C1)|(1<<LED_C2), (1<<LED_C1));
        }
        else{
            gpio_set_dir_masked((1<<LED_R1)|(1<<LED_R2), (1<<LED_R1));
            gpio_put_masked((1<<LED_C1)|(1<<LED_C2), (1<<LED_C2));
        }
        sleep_us(20);

    
    }
}
