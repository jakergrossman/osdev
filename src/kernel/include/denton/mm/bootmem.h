#ifndef __DENTON_MM_BOOTMEM_H
#define __DENTON_MM_BOOTMEM_H

#include <denton/types.h>

#include <stddef.h>
#include <stdint.h>

/* add a memory region for use by boot allocator */
int bootmem_add(uint64_t start, uint64_t end);

void* bootmem_alloc(size_t length, size_t alignment);

/* init page allocator. no more bootmem after call! */
void bootmem_setup_pga(void);

#endif
