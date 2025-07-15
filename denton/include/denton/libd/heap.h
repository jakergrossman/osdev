#ifndef __DENTON_KLIB_HEAP_H
#define __DENTON_KLIB_HEAP_H

#include <stdbool.h>
#include <stddef.h>

struct heap_node {
	struct heap_node* left;
	struct heap_node* right;
};

struct heap_head {
	size_t nr;
	struct heap_node * min;
};

struct heap_node_ops {
	bool (*cmp)(const void * lhs, const void * rhs);
};

static inline void __heap_swap(struct heap_node * lhs, struct heap_node * rhs)
{
	struct heap_node tmp = *lhs;
	*lhs = *rhs;
	*rhs = tmp;
}

#endif
