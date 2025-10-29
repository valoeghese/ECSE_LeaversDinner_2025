#include "Timer2.h"

#include "pico/stdlib.h"
#include <stdint.h>

static int64_t period_us = 100000;
static uint8_t enable = 0;
static void (*callback)(void);

static int64_t alarm_callback(alarm_id_t id, __unused void *user_data) {
    if (enable) {
        callback();
    }
    return period_us;
}

void Timer_2_Init(void (*callback_pass)(void)) {
    callback = callback_pass;
    add_alarm_in_ms(period_us, alarm_callback, NULL, false);
}

void Timer_2_Enable(void) {
    enable = 1;
}

void Timer_2_Sleep(void) {
    enable = 0;
}

void Timer_2_WritePeriod(uint16_t period) {
    period_us = (int64_t)period * 1000;
}

uint16_t Timer_2_ReadPeriod(void) {
    return (uint16_t) (period_us / 1000);
}