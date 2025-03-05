#include "asm/bitops.h"
#include "asm/findbit.h"
#include <denton/bma.h>
#include <denton/bits.h>

static void __bma_alloc(void* bma, size_t pos, size_t count)
{
	unsigned long* pbase = bma;
	for (size_t i = 0; i < count; i++) {
		set_bit(pos+i, pbase);
	}
}

long bma_alloc(void* bma, size_t nitems)
{
	unsigned long* pbase = bma;
	long result = 0;
	do {
		result = find_next_zero_bit(pbase, nitems, result);
	} while ((result > 0) && (!test_and_set_bit(result, pbase)));

	if (result >= 0) {
		__bma_alloc(bma, result, 1);
	}

	return result;
}

static long
__bma_find_consecutive_pos(unsigned long* bits, size_t bma_bits, size_t bitlen)
{
	size_t start = 0;
	while ((start = find_next_zero_bit(bits, bma_bits, start)) != bma_bits) {
		size_t word_idx = start / __BITS_PER_LONG;
		size_t bit_idx = start % __BITS_PER_LONG;
		for (int i = bit_idx; i < bit_idx + bitlen; i++) {
			if (test_bit(i, bits + word_idx)) {
				start = start + i;
				goto next_run;
			}
		}
		return start;
next_run:
		;
	}

	return -1;
}

long bma_alloc_consecutive(void* bma, size_t bma_bits, size_t consecutive)
{
	long pos = __bma_find_consecutive_pos(bma, bma_bits, consecutive);
	if (pos >= 0) {
		__bma_alloc(bma, pos, consecutive);
	}
	return pos;
}

void bma_free(void* bma, long base, size_t count)
{
	unsigned long* pbase = bma;
	for (size_t i = 0; i < count; i++) {
		clr_bit(base+i, pbase);
	}
}
