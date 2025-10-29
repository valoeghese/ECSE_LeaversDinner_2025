// Tetris for Leavers Dinner Card

#include "tetris.hpp"
#include <stdint.h>
#include <string.h>
#include "matrix_display.hpp"

// Axes: v ->
static uint8_t screen[] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};

// [x][y]
static uint8_t world[5][5] = { 0 };
static uint8_t input_shape[3][3] = { 0 };
// bottom right of shape0.
static uint8_t input_x = 0;
static uint8_t input_y = 0;
static uint8_t input_width = 0;

static uint8_t shapes[] = {
    0b000111, // ---
    0b001111, // --/
    0b011011, //  []
    0b100111  // \--
};

static void AlignWidth(void) {
    uint8_t smallest_x = 1;
    uint8_t largest_x = 1;

    for (uint8_t y = 0; y < 2; y++) {
        for (uint8_t x = 0; x < 3; x++) {
            if (input_shape[x][y]) {
                if (x < smallest_x) {
                    smallest_x = x;
                }
                if (x > largest_x) {
                    largest_x = x;
                }
            }
        }
    }

    input_width = largest_x - smallest_x + 1;
    // assume aligned right, anchor bottom right
    // lock to left
    if (input_x < input_width - 1) {
        input_x = input_width - 1;
    }
}

static void AlignBottomRight(void)
{
    // check bottom row
    if (input_shape[0][2] == 0 &&
        input_shape[1][2] == 0 &&
        input_shape[2][2] == 0)
    {
        
        for (int8_t y = 2; y >= 1; y--) {
            input_shape[0][y] = input_shape[0][y - 1];
            input_shape[1][y] = input_shape[1][y - 1];
            input_shape[2][y] = input_shape[2][y - 1];
        }
        input_shape[0][0] = 0;
        input_shape[1][0] = 0;
        input_shape[2][0] = 0;
    }

    // check right row
    if (input_shape[2][0] == 0 &&
        input_shape[2][1] == 0 &&
        input_shape[2][2] == 0)
    {
        for (int8_t x = 2; x >= 1; x--) {
            input_shape[x][0] = input_shape[x - 1][0];
            input_shape[x][1] = input_shape[x - 1][1];
            input_shape[x][2] = input_shape[x - 1][2];
        }
        input_shape[0][0] = 0;
        input_shape[0][1] = 0;
        input_shape[0][2] = 0;   
    }
}

static void LoadShape(uint8_t idx)
{
    // Bit format: 0b ML MM MR BL BM BR
    // First bit in the loop is thus BR => (2, 2)
    for (int8_t y = 2, i = 0; y >= 1; y--) {
        for (int8_t x = 2; x >= 0; x--, i++) {
            input_shape[x][y] = (shapes[idx] >> i) & 1;
        }
    }
    input_shape[0][0] = 0;
    input_shape[1][0] = 0;
    input_shape[2][0] = 0;

    input_y = 3;
    input_x = 2;
    AlignWidth();
}

static void RotateShape(void)
{
    uint8_t temp_shape[3][3] = { 0 };
    // rotate into temp_shape
    for (uint8_t y = 0; y < 3; y++) {
        for (uint8_t x = 0; x < 3; x++) {
            temp_shape[x][y] = input_shape[2 - y][x];
        }
    }
    // memcpy
    memcpy(input_shape, temp_shape, 3 * 3);

    // Align
    AlignBottomRight();
    //AlignWidth();
}

// Implementation
static void OnButtonPress(gpio_input in)
{
    if (in == I_PB1) {
        RotateShape();
    }
    if (in == I_PB2) {
        input_x--;
        if (input_x == (uint8_t)255) {
            input_x = 5;
        }
    }
}

static void OnTick(void)
{
    
}

static void OnPreRender(void)
{
    // Compose screen from world and shape
    for (int8_t x = 0; x < 5; x++) {
        uint8_t compose = 0;
        for (int8_t y = 0; y < 5; y++) {
            compose <<= 1;
            compose |= world[x][y];

            // anchor is bottom right
            int8_t input_render_x = x - input_x + 2;
            int8_t input_render_y = y - input_y + 2;
            if (input_render_x >= 0 && input_render_y >= 0 && input_render_x < 3 && input_render_y < 3) {
                compose |= input_shape[input_render_x][input_render_y];
            }
        }
        screen[x] = compose;
    }
}

static void OnDrawFrame(void)
{
    disp_char(screen);
}

static void OnPostRender(void)
{
    
}

// Behaviour
struct behaviour tetris_behaviour = {
    OnButtonPress,
    OnTick,
    OnDrawFrame,
    OnPreRender,
    OnPostRender
};

void InitTetris(void)
{
    LoadShape(1);
}