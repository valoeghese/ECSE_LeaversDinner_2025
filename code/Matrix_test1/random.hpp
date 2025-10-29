#pragma once

#include <stdint.h>

uint64_t xorshift64star();
void xorshift64seed(uint64_t seed);