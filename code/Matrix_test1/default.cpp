// Default scroll behaviour for leavers card

#include "default.hpp"

#include <stdint.h>
#include <string.h>
#include "pico_flash.hpp"
#include "matrix_display.hpp"
#include "pindefs.hpp"

#include "tetris.hpp"
#include "random.hpp"

enum disp_mode{
    USER = 0,
    ECSE = 1,
    EASTER = 2
};

disp_mode display_mode = USER;

#define STR_BUFFER_LEN 128

// Scroll
uint counter = 0;
uint scroll_count = 0;
const uint8_t * current_char;

// Buffers
static char userStringBuffer[STR_BUFFER_LEN] = " Use PuTTY to Program (115200b)";
static char presetStringBuffer[STR_BUFFER_LEN] = " ECSE LEAVERS 2025";
static char easterEggStr[STR_BUFFER_LEN] = " COMPSYS ON TOP";

static char * dispBuff = userStringBuffer;

static char * strings[] = {
    userStringBuffer,
    presetStringBuffer,
    easterEggStr
};

// Implementation
static void OnButtonPress(gpio_input in)
{
    switch (in) {
    case I_PB1:
    // enter tetris
        xorshift64seed(time_us_32());
        InitTetris(); // set up tetris
        current_behaviour = tetris_behaviour;
        break;
    case I_PB2:
    // default modes
        static bool toggle = 0;
        display_mode = (toggle ^= 1) ? USER : ECSE;
        break;
    case I_PB1_PB2:
    // easter egg
        display_mode = EASTER;
        break;
    }

    //3MSB of first col of char = length (0-7)
    counter = 0;

    const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
    add_char_to_scroll_start(disp_char);
    add_char_to_scroll(disp_char);
    scroll_count=5-((disp_char[0]&0xE0)>>5);
}

static void OnTick(void)
{
    scroll_chars();
    scroll_count++;

    if(scroll_count==7){
        counter = (counter+1) % strlen(strings[display_mode]);
        const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
        add_char_to_scroll(disp_char);
        scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
    }
}

static void OnPreRender(void)
{
    // Set character to display
    if(gpio_get(PB2)){
        current_char=scroll_buff;
    }
}

static void OnDrawFrame(void)
{
    disp_char(current_char);
}

static void OnPostRender(void)
{
    // Nothing
}

// Behaviour
struct behaviour default_behav = {
    OnButtonPress,
    OnTick,
    OnDrawFrame,
    OnPreRender,
    OnPostRender
};

// API Functions & their implementation

static void screen_start(void){
    const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
    add_char_to_scroll(disp_char);
    scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
}

void DefaultBehaviourInit(void)
{
    read_name_from_flash(userStringBuffer, STR_BUFFER_LEN);
    screen_start();
}

void DefaultBehaviourDebug(const char *tempBuffer) {
    printf("Displaying String \"%s\"\n", tempBuffer);
    memccpy(userStringBuffer, tempBuffer, 0, 64);
    uint rc = write_name_to_flash(userStringBuffer);
    if(!rc){
        printf("wrote string \"%s\" (%d bytes) to flash\n", userStringBuffer, strlen(userStringBuffer)+1);
    }
    counter = 0;
}