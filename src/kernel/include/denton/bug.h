#ifndef __DENTON_BUG_H
#define __DENTON_BUG_H

#include <denton/compiler.h>

/* a place for all the bugs to live, so they don't infest our program :) */

#define BUILD_BUG_ON_ZERO(expr) \
    ((int)(sizeof(struct { int: (-!!(expr)); })))

/* permits the compiler to check the validity of the expression
 * without code generation or side effects */
#define BUILD_BUG_ON_INVALID(expr) \
    ((void)(sizeof((__force long)))

#endif
 
 
