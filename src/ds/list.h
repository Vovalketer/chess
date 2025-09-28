/*
 * List used in the linux kernel
 */
#ifndef LIST_H
#define LIST_H

#include "container_of.h"

struct list_node {
	struct list_node *next;
	struct list_node *prev;
};

#define LIST_HEAD_INIT(name) {.next = &name, .prev = &name}
#define LIST_HEAD(name)		 struct list_node name = LIST_HEAD_INIT(name)

#define list_entry(ptr, type, member) container_of(ptr, type, member)

static inline void list_init(struct list_node *list) {
	list->next = list;
	list->prev = list;
}

static inline void list_add_between(struct list_node *new,
									struct list_node *prev,
									struct list_node *next) {
	new->next  = next;
	new->prev  = prev;
	prev->next = new;
	next->prev = new;
}

static inline void list_add(struct list_node *new, struct list_node *head) {
	list_add_between(new, head, head->next);
}

static inline void list_add_tail(struct list_node *new, struct list_node *head) {
	list_add_between(new, head->prev, head);
}

static inline void list_del(struct list_node *entry) {
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
}

static inline void list_replace(struct list_node *new, struct list_node *old) {
	new->next		= old->next;
	new->next->prev = new;
	new->prev		= old->prev;
	new->prev->next = new;
}

static inline void list_swap(struct list_node *a, struct list_node *b) {
	struct list_node *tmp = a->next;
	list_replace(a, b);
	list_add_between(b, tmp->prev, tmp);
}

static inline void list_move(struct list_node *list, struct list_node *head) {
	list_del(list);
	list_add(list, head);
}

static inline void list_move_tail(struct list_node *list, struct list_node *head) {
	list_del(list);
	list_add_tail(list, head);
}

static inline void list_splice_between(struct list_node *list,
									   struct list_node *next,
									   struct list_node *prev) {
	struct list_node *first = list->next;
	struct list_node *last	= list->prev;
	first->prev				= prev;
	prev->next				= first;
	last->next				= next;
	next->prev				= last;
}

static inline void list_splice(struct list_node *list, struct list_node *head) {
	list_splice_between(list, head->next, head);
}

static inline void list_splice_tail(struct list_node *list, struct list_node *head) {
	list_splice_between(list, head, head->prev);
}

static inline int list_empty(struct list_node *head) {
	return head->next == head;
}

static inline int list_is_head(struct list_node *entry, struct list_node *head) {
	return entry == head;
}

#define list_first_entry(ptr, type, member)	  list_entry((ptr)->next, type, member)
#define list_last_entry(ptr, type, member)	  list_entry((ptr)->prev, type, member)
#define list_entry_is_head(pos, head, member) list_is_head(&(pos)->member, (head))

#define list_next_entry(pos, member) list_entry((pos)->member.next, __typeof__(*(pos)), member)
#define list_prev_entry(pos, member) list_entry((pos)->member.prev, __typeof__(*(pos)), member)

#define list_for_each(pos, head) \
	for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)
// #define list_for_each(pos, head) for (pos = !list_is_head(pos, (head)); pos = pos->next)
#define list_for_each_entry(pos, head, member)                   \
	for (pos = list_first_entry(head, __typeof__(*pos), member); \
		 !list_entry_is_head(pos, head, member);                 \
		 pos = list_next_entry(pos, member))

#define list_for_each_reverse(pos, head) \
	for (pos = (head)->prev; !list_is_head(pos, (head)); pos = pos->prev)
#define list_for_each_entry_reverse(pos, head, member)          \
	for (pos = list_last_entry(head, __typeof__(*pos), member); \
		 !list_entry_is_head(pos, head, member);                \
		 pos = list_prev_entry(pos, member))
#endif	// LIST_H
