#include <stdint.h>

void Timer_2_Init(void (*callback_pass)(void));

void Timer_2_Enable(void);
void Timer_2_Sleep(void);
void Timer_2_WritePeriod(uint16_t period);
uint16_t Timer_2_ReadPeriod(void);
