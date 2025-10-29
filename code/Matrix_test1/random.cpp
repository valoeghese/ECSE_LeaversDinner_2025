#include "random.hpp"

#include <stdint.h>

// xorshift64* PRNG
static uint64_t prng_state = 0;
uint64_t xorshift64star() {
    uint64_t x = prng_state;
    if (x == 0) x = 0xdeadbeefcafef00dULL; // never zero
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    prng_state = x;
    return x * 0x2545F4914F6CDD1DULL;
}
void xorshift64seed(uint64_t seed)
{
    prng_state = seed;
}