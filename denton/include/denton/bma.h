#ifndef __DENTON_BMA_H
#define __DENTON_BMA_H

#include <stddef.h>

/**
 * bma_alloc() - allocate a single bit from the bitmap
 * @bma: pointer to memory where bitmap is stored
 * @bma_len: number of bits in bitmap (alternatively, maximum bits to search)
 *
 * Returns -1 if there are no free bits, and the bit position on success
 */
long bma_alloc(void* bma, size_t bma_len);

/**
 * bma_alloc_consecutive() - allocate consecutive bits from the bitmap
 * @bma: pointer to memory where bitmap is stored
 * @bma_len: number of bits in bitmap (alternatively, maximum bits to search)
 * @bit_len: consecutive bit count
 *
 * Returns -1 if there are no runs of @bit_len bits, and the first bit position on success
 */
long bma_alloc_consecutive(void* bma, size_t bma_len, size_t bit_len);

/** bma_free() - mark @count bits starting at @base as free */
void bma_free(void* bma, long base, size_t count);

#endif
