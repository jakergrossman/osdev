#ifndef __DENTON_MINMAX_H
#define __DENTON_MINMAX_H

#include <denton/compiler.h>

#define __cmp(op, x, y) ((x) op (y) ? (x) : (y))

#define __careful_cmp_once(op, x, y, ux, uy) ({ \
		__auto_type ux = (x); \
		__auto_type uy = (y); \
		__cmp(op, ux, uy); \
})

#define __careful_range(x, lo, hi) \
	__careful_range_once(x, lo, hi, __UNIQ_ID(x_), __UNIQ_ID(lo_), __UNIQ_ID(hi_))

#define __careful_cmp(op, x, y) \
	__careful_cmp_once(op, x, y, __UNIQ_ID(x_), __UNIQ_ID(y_))

#define kmin(x, y) __careful_cmp(<, x, y)
#define kmax(x, y) __careful_cmp(>, x, y)

#endif
