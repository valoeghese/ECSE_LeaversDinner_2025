#include <stdint.h>

#define TONEGEN_PWM_PIN 1

void ToneGen_Init(void);

void ToneGen_Enable(void);
void ToneGen_Sleep(void);
void ToneGen_WritePeriod(uint16_t period);
void ToneGen_WriteCompare(uint16_t compare);
