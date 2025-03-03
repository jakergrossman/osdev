#ifndef __DENTON_ERR_H
#define __DENTON_ERR_H

#include <denton/compiler.h>
#include <denton/types.h>

/**
 * kernel pointers are page aligned, so the bottom 12 bits can be
 * stuffed to return errors and pointers in the same result
 */
#define MAX_ERRNO 4096

#ifndef __ASSEMBLY__

static inline bool IS_ERR_UNCHECKED(const void * ptr)
{
    return unlikely((unsigned long)(void*)ptr) >= (unsigned long)-MAX_ERRNO;
}

static inline bool __must_check IS_ERR(const void * ptr)
{
    return IS_ERR_UNCHECKED(ptr);
}

static inline long __must_check PTR_ERR(const void* ptr)
{
    return (long)ptr;
}

static inline void * __must_check ERR_PTR(unsigned long ptr)
{
    return (void*)ptr;
}

static inline bool __must_check IS_ERR_OR_NULL(const void * ptr)
{
    return unlikely(!ptr) || IS_ERR_UNCHECKED(ptr);
}

/* explicitly cast an error-valued pointer to another type */
static inline void * __must_check ERR_CAST(const void* ptr)
{
    /* cast away the const */
    return (void*) ptr;
}

#endif

#endif
