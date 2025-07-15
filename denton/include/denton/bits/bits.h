#ifndef __DENTON_BITS_BITS_H
#define __DENTON_BITS_BITS_H

#include <denton/bug.h>
#include <denton/compiler.h>

#ifndef __BITS_PER_LONG
#  if defined(__CHAR_BIT__) && defined(__SIZEOF_LONG__)
#	 define __BITS_PER_LONG (__CHAR_BIT__ * __SIZEOF_LONG__)
#  else
#	 define __BITS_PER_LONG (32)
#  endif
#endif

#ifndef __BITS_PER_LONG_LONG
#define __BITS_PER_LONG_LONG (64)
#endif


/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/* bits.h: Macros for dealing with bitmasks.  */
#define __GENMASK(h, l) \
		(((~0UL) - (1UL << (l)) + 1) & \
		 (~0UL >> (__BITS_PER_LONG - 1 - (h))))

#define __GENMASK_ULL(h, l) \
		(((~0ULL) - (1ULL << (l)) + 1) & \
		 (~0ULL >> (__BITS_PER_LONG_LONG - 1 - (h))))

#if !defined(__ASSEMBLY)
#define __GENMASK_INPUT_CHECK(h, l) \
	(BUILD_BUG_ON_ZERO(__builtin_choose_expr( \
		__is_constexpr((l) > (h)), (l) > (h), 0)))
#else
/* BUILD_BUG_ON_ZERO not available in H files from ASM */
#define __GENMASK_INPUT_CHECK(h, l) 0
#endif

#define GENMASK(h, l) \
	(__GENMASK_INPUT_CHECK(h, l) + __GENMASK(h, l))

#define GENMASK_ULL(h, l) \
	(__GENMASK_INPUT_CHECK(h, l) + __GENMASK_ULL(h, l))

#define BIT(nr)             (1UL << (nr))
#define BIT_MASK(width)     (1UL << ((nr) & __BITS_PER_LONG))
#define BIT_WORD(nr)        ((nr) / __BITS_PER_LONG)
#define BIT_ULL_MASK(width) (1ULL << ((nr) & __BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)    ((nr) / __BITS_PER_LONG_LONG)
#define BITS_PER_BYTPE      8

#endif
