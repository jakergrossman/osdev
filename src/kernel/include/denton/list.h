#ifndef __DENTON_LIST_H
#define __DENTON_LIST_H

#include <denton/container_of.h>

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

#define list_entry(iter, type, member) \
    container_of(iter, type, member)

#define list_for_each(iter, head) \
    for (iter = (head)->next; iter != (head); iter = iter->next)

#define list_for_each_entry(iter, head, member) \
    for (iter = list_entry((head)->next, typeof(*iter), member); \
         &iter->member != (head); \
         iter = list_entry(iter->member.next, typeof(*iter), member))

#endif
