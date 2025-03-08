#ifndef __DENTON_MM_PGF_H
#define __DENTON_MM_PGF_H

enum {
	__PGF_ALIGN_BITS = 5,
	/* pages that can be freed via slab allocator strinkers */
	__PGF_RECLAIMABLE_BIT = __PGF_ALIGN_BITS,
	/* pages that we intend to dirty */
	__PGF_WRITE,
};

#define __GFP_RECLAIMABLE

#endif
