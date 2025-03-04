#ifndef __DENTON_BITS_BITMAP_H
#define __DENTON_BITS_BITMAP_H

#include <denton/compiler.h>
#include <denton/bits/bits.h>

#define BITMAP_DECL(name, bits) \
        unsigned long name [((bits) + (__BITS_PER_LONG-1)) / __BITS_PER_LONG]

#endif
