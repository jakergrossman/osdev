#include <stdint.h>

static uint32_t xorshift_rand(uint32_t* state)
{
	uint32_t *p = (uint32_t*)state;
	uint32_t x = *p;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*p = x;
	return x;
}

static uint32_t rand_state = 1337;

void srand(unsigned int seed)
{
	rand_state = seed;
	(void)xorshift_rand(&rand_state);
}

int rand(void)
{
	return xorshift_rand(&rand_state);
}
