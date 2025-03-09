#ifndef __DENTON_RING_h
#define __DENTON_RING_h

#include "denton/compiler.h"
#include <denton/spinlock.h>
#include <denton/stddef.h>

#include <stddef.h>

struct raw_ringbuffer {
	void* mem;
	size_t len;
	size_t tail;
	size_t head;
};

struct ringbuffer {
	struct raw_ringbuffer raw;
	spinlock_t lock;
};

static __always_inline struct raw_ringbuffer
__RING_INIT(void* memory, size_t size)
{
	return (struct raw_ringbuffer){
		.mem = memory,
		.len = size,
		.tail = 0,
		.head = 0,
	};
}

static inline void
__ring_init(struct raw_ringbuffer* ring, void* memory, size_t size)
{
	if (memory && size) {
		*ring = __RING_INIT(memory, size);
	}
}

/**
 * __ring_put() - place up to @len bytes from @memory into @ring
 * Returns the number of elements inserted
 */
size_t __ring_put(struct raw_ringbuffer* ring, const void* memory, size_t len);

/**
 * __ring_put() - place up to @len bytes from @memory into @ring
 * Returns the number of elements inserted
 */
size_t __ring_put_exact(struct raw_ringbuffer* ring, const void* memory, size_t len);

size_t __ring_get(struct raw_ringbuffer* ring, void* memory, size_t len);
size_t __ring_get_exact(struct raw_ringbuffer* ring, void* memory, size_t len);

void   __ring_flush(struct raw_ringbuffer* ring);
size_t __ring_count(const struct raw_ringbuffer* ring);
size_t __ring_count_to_end(const struct raw_ringbuffer* ring);
size_t __ring_space(const struct raw_ringbuffer* ring);
size_t __ring_space_to_end(const struct raw_ringbuffer* ring);

static __always_inline struct ringbuffer
RING_INIT(void* memory, size_t size)
{
	return (struct ringbuffer){
		.raw = __RING_INIT(memory, size),
		.lock = SPINLOCK_INIT(0),
	};
}

static inline void
ring_init(struct ringbuffer* ring, void* memory, size_t size)
{
	if (memory && size) {
		*ring = RING_INIT(memory, size);
	}
}

static inline int
ring_put(struct ringbuffer * ring, void * memory, size_t len)
{
	using_spin_lock(&ring->lock) {
		return __ring_put(&ring->raw, memory, len);
	}
}

static inline int
ring_get(struct ringbuffer * ring, void * memory, size_t len)
{
	using_spin_lock(&ring->lock) {
		return __ring_get(&ring->raw, memory, len);
	}
}

static inline void
ring_flush(struct ringbuffer * ring)
{
	using_spin_lock(&ring->lock) {
		__ring_flush(&ring->raw);
	}
}

static inline size_t
ring_count(struct ringbuffer * ring)
{
	using_spin_lock(&ring->lock) {
		return __ring_count(&ring->raw);
	}
}

static inline size_t
ring_count_to_end(struct ringbuffer * ring)
{
	using_spin_lock(&ring->lock) {
		return __ring_count_to_end(&ring->raw);
	}
}

static inline size_t
ring_space(struct ringbuffer * ring)
{
	using_spin_lock(&ring->lock) {
		return __ring_space(&ring->raw);
	}
}

static inline size_t
ring_space_to_end(struct ringbuffer * ring)
{
	using_spin_lock(&ring->lock) {
		return __ring_space_to_end(&ring->raw);
	}
}


#endif
