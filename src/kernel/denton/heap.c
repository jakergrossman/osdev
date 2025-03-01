#include <stddef.h>
#include <stdint.h>

#include <denton/bits.h>

#include "heap.h"

extern const uintptr_t __KERNEL_END;
static uintptr_t bump_ptr = (uintptr_t)(&__KERNEL_END);

// TODO: real allocator
void* kmalloc(size_t size, uint32_t flags, uintptr_t* phys_out)
{
    const uint32_t align_shift = (flags & __GFP_ALIGN_MASK);
    const uint32_t align       = BIT(align_shift);
    const uint32_t align_mask  = __GENMASK(__BITS_PER_LONG-1, align_shift);
    (void)flags;

#include <stdio.h>
    printf("%d ", BIT(align_shift));

    if ((align_shift != 0) && (bump_ptr & ~align_mask)) {
        // align to next 2^align_shift boundary
        bump_ptr &= align_mask;
        bump_ptr += align;
    }

    if (phys_out)
    {
        *phys_out = bump_ptr;
    }

    void* mem = (void*)bump_ptr;
    bump_ptr += size;
    return mem;
}

void* kzalloc(size_t size, uint32_t flags, uintptr_t* phys_out)
{
}
