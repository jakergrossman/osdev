#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

int rand(void);
void srand(unsigned int seed);

#ifdef __cplusplus
#define NULL nullptr
#else
#define NULL ((void*)0)
#endif

long strtol(const char* str, char** endptr, int base);
long strntol(const char* str, size_t maxlen, char** endptr, int base);
unsigned long strtoul(const char* str, char** endptr, int base);

/* Allocate @__size bytes of memory */
extern void* malloc(size_t __size)
	__attribute_malloc__ __attribute_alloc_size__((1));

/* Allocate @__elems zero-initialized elements, each @__elem_size bytes in size */
extern void* calloc(size_t __elems, size_t __elem_size)
	__attribute_malloc__ __attribute_alloc_size__((2));

/* Reallocate previously allocated @__ptr, making the new block @__size bytes long */
extern void* realloc(void* __ptr, size_t __size)
	__attribute_malloc__ __attribute_alloc_size__((2));

#ifdef __cplusplus
}
#endif

#endif
