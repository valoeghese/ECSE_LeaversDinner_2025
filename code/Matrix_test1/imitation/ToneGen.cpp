#include "ToneGen.hpp"
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"

uint slice_num, slice_chan;

#define CLOCK_FREQ 125000000
#define DIVIDER 1250  // 100 kHz

void ToneGen_Init(void)
{
    gpio_set_function(TONEGEN_PWM_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(TONEGEN_PWM_PIN);
    slice_chan = pwm_gpio_to_channel(TONEGEN_PWM_PIN);
    ToneGen_WritePeriod(1000);
    ToneGen_WriteCompare(500);
    
    pwm_set_clkdiv(slice_num, DIVIDER);
}

void ToneGen_Enable(void)
{
    pwm_set_enabled(slice_num, 1);
}

void ToneGen_Sleep(void)
{
    pwm_set_enabled(slice_num, 0);
}

void ToneGen_WritePeriod(uint16_t period)
{
    pwm_set_wrap(slice_num, period);
}

void ToneGen_WriteCompare(uint16_t compare)
{
    pwm_set_chan_level(slice_num, slice_chan, compare);
}