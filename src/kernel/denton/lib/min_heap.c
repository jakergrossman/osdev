#include <denton/lib/min_heap.h>
#include <stdlib.h>

void __min_heap_init(min_heap_char * heap, void * data, size_t size)
{
	heap->count = 0;
	heap->max = size;
	heap->data = data;
}

void * __min_heap_peek(min_heap_char * heap)
{
	/* first element is min element */
	return (heap->count > 0) ? heap->data : NULL;
}

bool __min_heap_isfull(min_heap_char * heap)
{
	return heap->count == heap->max;
}

void * __min_heap_pop(min_heap_char * heap, size_t elem_size)
{
	void* data = heap->data;

#if 0
	if (WARN_ONCE(heap->count <= 0, "Popping an empty heap")) {
		return NULL;
	}
#endif

}

void   __min_heap_push(min_heap_char * heap, const void * elem, size_t elem_size);
void * __min_heap_pop_push(min_heap_char * heap, const void * elem, size_t elem_size);
void * __min_heap_del(min_heap_char * heap, size_t elem_idx, size_t elem_size);

