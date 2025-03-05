#ifndef __DENTON_BMA_H
#define __DENTON_BMA_H

#include <stddef.h>

long bma_alloc(void* bma, size_t nitems);
long bma_alloc_consecutive(void* bma, size_t nbits, size_t bitlen);
void bma_free(void* bma, long base, size_t count);

#endif
