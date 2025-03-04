#include <denton/ring.h>

#include <string.h>

size_t __ring_put(struct raw_ringbuffer* ring, const void* memory, size_t len)
{
    size_t copysize = kmin(len, __ring_space(ring));
    if (copysize > 0) {
        size_t back_size = kmin(copysize, __ring_space_to_end(ring));
        memcpy(ring->mem + ring->head, memory, back_size);
        if (back_size < copysize) {
            size_t front_size = copysize - back_size;
            memcpy(ring->mem, memory + back_size, front_size);
        }
    }
    ring->head = (ring->head + copysize) & (ring->len-1);
    return copysize;
}

size_t __ring_put_exact(struct raw_ringbuffer* ring, const void* memory, size_t len)
{
    if (__ring_space(ring) >= len) {
        size_t back_size = kmin(len, __ring_space_to_end(ring));
        memcpy(ring->mem + ring->head, memory, back_size);
        if (back_size < len) {
            size_t front_size = len - back_size;
            memcpy(ring->mem, memory + back_size, front_size);
        }
    }
    ring->head = (ring->head + len) & (ring->len-1);
    return len;
}

size_t __ring_get(struct raw_ringbuffer* ring, void* memory, size_t len)
{
    size_t copysize = kmin(len, __ring_count(ring));
    if (copysize > 0) {
        size_t back_size = kmin(copysize, __ring_count_to_end(ring));
        memcpy(memory, ring->mem + ring->tail, back_size);
        if (back_size < copysize) {
            size_t front_size = copysize - back_size;
            memcpy(memory + back_size, ring->mem, front_size);
        }
    }
    ring->tail = (ring->tail + copysize) & (ring->len-1);
    return copysize;
}

size_t __ring_get_exact(struct raw_ringbuffer* ring, void* memory, size_t len)
{
    if (__ring_count(ring) >= len) {
        size_t back_size = kmin(len, __ring_count_to_end(ring));
        memcpy(memory, ring->mem + ring->tail, back_size);
        if (back_size < len) {
            size_t front_size = len - back_size;
            memcpy(memory + back_size, ring->mem, front_size);
        }
    }
    ring->tail = (ring->tail + len) & (ring->len-1);
    return len;
}

void __ring_flush(struct raw_ringbuffer* ring)
{
    ring->tail = ring->head = 0;
}

size_t __ring_count(const struct raw_ringbuffer* ring)
{
    return ((ring->head - ring->tail) & (ring->len-1));
}

size_t __ring_count_to_end(const struct raw_ringbuffer* ring)
{
    size_t end = ring->len - ring->tail;
    int n = (end + ring->head) & (ring->len-1);
    return n <= end ? n : end;
}

size_t __ring_space(const struct raw_ringbuffer* ring)
{
    return ((ring->tail - (ring->head + 1)) & (ring->len-1));
}

size_t __ring_space_to_end(const struct raw_ringbuffer* ring)
{
    size_t end = ring->len - 1 - ring->head;
    int n = (end + ring->tail) & (ring->len-1);
    return n <= end ? n : end+1;
}

