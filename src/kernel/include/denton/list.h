#ifndef __DENTON_LIST_H
#define __DENTON_LIST_H

#include "denton/compiler.h"
#include <denton/container_of.h>
#include <stdbool.h>

struct list_head {
	struct list_head* prev;
	struct list_head* next;
};

static inline void
list_init(struct list_head* list)
{
	list->next = list;
	list->prev = list;
}

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void
__list_add(struct list_head* added,
		   struct list_head* prev,
		   struct list_head* next)
{
	next->prev = added;
	added->next = next;

	added->prev = prev;
	prev->next = added;
}

static inline void
list_add(struct list_head* added, struct list_head* head)
{
	__list_add(added, head, head->next);
}

static inline void
list_add_tail(struct list_head* added, struct list_head* head)
{
	__list_add(added, head->prev, head);
}


static inline void
__list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void
list_del(struct list_head * entry)
{
	__list_del(entry->prev, entry->next);
}

static inline bool
list_empty(struct list_head * entry)
{
	return entry->next == entry;
}

static inline void
list_rotate(struct list_head * entry)
{
	struct list_head* head = entry;
	struct list_head* prev = head->prev;
	list_del(entry);
	list_add_tail(head, prev);
}

/* return whether @entry is placed in a list */
static inline bool
list_placed_in_list(struct list_head * entry)
{
	return !list_empty(entry);
}

#define list_entry(iter, type, member) \
	container_of(iter, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr)? list_first_entry(ptr, type, member): NULL)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

#define list_for_each(iter, head) \
	for (iter = (head)->next; iter != (head); iter = iter->next)

#define list_for_each_entry(iter, head, member) \
	for (iter = list_entry((head)->next, typeof(*iter), member); \
		 &iter->member != (head); \
		 iter = list_entry(iter->member.next, typeof(*iter), member))


#define list_for_each_entry_safe(head, pos, nxt, member) \
	for (pos = list_first_entry(head, typeof(*pos), member), \
			nxt = list_next_entry(pos, member); \
		 &pos->member != (head); \
		 pos = nxt, nxt = list_next_entry(nxt, member))

#endif
