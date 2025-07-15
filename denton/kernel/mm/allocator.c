

#include <stddef.h>
#include <denton/mm/allocator.h>
#include <denton/mm/pga.h>

void* kmem_alloc_ally(const struct allocator* ally, size_t len, pgf_t flags)
{
	return ally->ops->alloc(ally->ptr, len, flags);
}

void kmem_free_ally(const struct allocator* ally, void* memory)
{
	if (ally->ops->free) {
		ally->ops->free(ally->ptr, memory);
	}
}
