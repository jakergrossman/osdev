// SPDX-License-Identifier: GPL-2.0-or-later
/* bit search implementation
 *
 * Copied from lib/find_bit.c to tools/lib/find_bit.c
 *
 * Copyright (C) 2004 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * Copyright (C) 2008 IBM Corporation
 * 'find_last_bit' is written by Rusty Russell <rusty@rustcorp.com.au>
 * (Inspired by David Howell's find_next_bit implementation)
 *
 * Rewritten by Yury Norov <yury.norov@gmail.com> to decrease
 * size and improve performance, 2015.
 */
#ifndef _KERNEL_1386_ASM_FINDBIT_H
#define _KERNEL_1386_ASM_FINDBIT_H

#include "bitops.h"

/*
 * Common helper for find_bit() function family
 * @FETCH: The expression that fetches and pre-processes each word of bitmap(s)
 * @MUNGE: The expression that post-processes a word containing found bit (may be empty)
 * @size: The bitmap size in bits
 */
#define _FIND_FIRST_BIT(FETCH, MUNGE, size)					\
({										\
	unsigned long idx, val, sz = (size);					\
										\
	for (idx = 0; idx * __BITS_PER_LONG < sz; idx++) {			\
		val = (FETCH);							\
		if (val) {							\
			sz = min(idx * __BITS_PER_LONG + __ffs(MUNGE(val)), sz);	\
			break;							\
		}								\
	}									\
										\
	sz;									\
})

/*
 * Common helper for find_next_bit() function family
 * @FETCH: The expression that fetches and pre-processes each word of bitmap(s)
 * @MUNGE: The expression that post-processes a word containing found bit (may be empty)
 * @size: The bitmap size in bits
 * @start: The bitnumber to start searching at
 */
#define _FIND_NEXT_BIT(FETCH, MUNGE, size, start)				\
({										\
	unsigned long mask, idx, tmp, sz = (size), __start = (start);		\
										\
	if (unlikely(__start >= sz))						\
		goto out;							\
										\
	mask = MUNGE(~0UL << ((__start) & (__BITS_PER_LONG - 1)));				\
	idx = __start / __BITS_PER_LONG;						\
										\
	for (tmp = (FETCH) & mask; !tmp; tmp = (FETCH)) {			\
		if ((idx + 1) * __BITS_PER_LONG >= sz)				\
			goto out;						\
		idx++;								\
	}									\
										\
	sz = min(idx * __BITS_PER_LONG + __ffs(MUNGE(tmp)), sz);			\
out:										\
	sz;									\
})

/*
 * Find the first set bit in a memory region.
 */
static inline unsigned long
find_first_bit(const unsigned long *addr, unsigned long size)
{
	return _FIND_FIRST_BIT(addr[idx], /* nop */, size);
}

/*
 * Find the first set bit in two memory regions.
 */
static inline unsigned long
find_first_and_bit(const unsigned long *addr1,
				  const unsigned long *addr2,
				  unsigned long size)
{
	return _FIND_FIRST_BIT(addr1[idx] & addr2[idx], /* nop */, size);
}

/*
 * Find the first cleared bit in a memory region.
 */
static inline unsigned long 
find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	return _FIND_FIRST_BIT(~addr[idx], /* nop */, size);
}

static inline unsigned long
find_next_bit(const unsigned long *addr, unsigned long nbits, unsigned long start)
{
	return _FIND_NEXT_BIT(addr[idx], /* nop */, nbits, start);
}

static inline unsigned long
_find_next_and_bit(const unsigned long *addr1, const unsigned long *addr2,
					unsigned long nbits, unsigned long start)
{
	return _FIND_NEXT_BIT(addr1[idx] & addr2[idx], /* nop */, nbits, start);
}

static inline unsigned long 
find_next_zero_bit(const unsigned long *addr, unsigned long nbits,
					 unsigned long start)
{
	return _FIND_NEXT_BIT(~addr[idx], /* nop */, nbits, start);
}

#endif
