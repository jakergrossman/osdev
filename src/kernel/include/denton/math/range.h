#ifndef __DENTON_RANGE_H
#define __DENTON_RANGE_H

#include <denton/compiler.h>

#include <stdint.h>

#define __careful_range_once(x, lo, hi, ux, ulo, uhi) ({ \
    __auto_type ux = (x); \
    __auto_type ulo = (lo); \
    __auto_type uhi = (hi); \
    ((ulo <= ux) && (ux < uhi)); \
})

#define __careful_range(x, lo, hi) \
    __careful_range_once(x, lo, hi, __UNIQ_ID(x_), __UNIQ_ID(lo_), __UNIQ_ID(hi_))

#define kinrange(x, lo, hi) __careful_range(x, lo, hi)

#endif
