#ifndef __DENTON_KLIB_MINHEAP_H
#define __DENTON_KLIB_MINHEAP_H


#include <stdbool.h>
#include <stddef.h>

struct min_heap;

#define MIN_HEAP_TYPE(__type, __name) \
struct __name { \
	size_t max; \
	size_t count; \
	__type * data; \
}

typedef MIN_HEAP_TYPE(char, min_heap_char) min_heap_char;

#define __min_heap_cast(__heap) (typeof((__heap)->data[0])*)
#define __min_heap_objsize(__heap) (sizeof((__heap)->data[0]))

void   __min_heap_init(min_heap_char * heap, void * data, size_t size);
void * __min_heap_peek(min_heap_char * heap);
bool   __min_heap_isfull(min_heap_char * heap);
void * __min_heap_pop(min_heap_char * heap, size_t elem_size);
void   __min_heap_push(min_heap_char * heap, const void * elem, size_t elem_size);
void * __min_heap_pop_push(min_heap_char * heap, const void * elem, size_t elem_size);
void * __min_heap_del(min_heap_char * heap, size_t elem_idx, size_t elem_size);

#define min_heap_init(__heap, __data, __size) \
	__min_heap_init((min_heap_char *)__heap, __data, __size)

#define min_heap_peek(__heap) \
	(__minheap_cast(__heap) __min_heap_peek((min_heap_char*__heap)))

#define min_heap_pop(__heap) \
	__min_heap_pop((min_heap_char *)__heap, __min_heap_objsize(__heap))

#define min_heap_push(__heap, __elem) \
	__min_heap_push((min_heap_char *)__heap, __elem, __min_heap_objsize(__heap))

#define min_heap_pop_push(__heap, __elem) \
	__min_heap_pop_push((min_heap_char *)__heap, __elem, __min_heap_objsize(__heap))

#endif
