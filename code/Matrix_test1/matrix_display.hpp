#ifndef MATRIX_DISPLAY_HPP
#define MATRIX_DISPLAY_HPP
#include <stdio.h>
#include <pico/stdlib.h>

const uint8_t* char_to_matrix(const char charIn);
void disp_char(const uint8_t * character);

#endif