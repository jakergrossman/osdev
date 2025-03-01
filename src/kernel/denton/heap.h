#ifndef _KERNEL_HEAP_H
#define _KERNEL_HEAP_H

#include <stddef.h>
#include <stdint.h>
#define __GFP_ALIGN_MASK      __GENMASK(4, 0)
#define GFP_ALIGN(n)        ((n) & __GFP_ALIGN_MASK)
#define GFP_KERNEL          BIT(5)
#define GFP_USER            BIT(6)

void* kmalloc(size_t size, uint32_t flags, uintptr_t* phys_out);

#endif
