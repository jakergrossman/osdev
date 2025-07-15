#ifndef _STDDEF_H
#define _STDDEF_H

typedef __SIZE_TYPE__ size_t;

typedef typeof((int*)0 - (int*)0) ptrdiff_t;

#endif
