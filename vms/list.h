#ifndef _LIST_H_
#define _LIST_H_

#define LIST_NULL	0

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name)	{ &(name), &(name) }

#define LIST_HEAD(name)		struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr)	do{(ptr)->next = (ptr); (ptr)->prev = (ptr);}while(0)

#define offsetof(type, member)	((char *)&((type *)0)->member)

#define container_of(ptr, type, member) ((type *)((char *)ptr - offsetof(type, member)))


static void __list_add(struct list_head *_new,
			struct list_head *prev, struct list_head *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;	
	prev->next = _new;
}

static void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static void list_add(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head, head->next);
}

static void list_add_tail(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head->prev, head);
}

static void list_move(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

static void list_move_tail(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

static void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = LIST_NULL;
	entry->prev = LIST_NULL;
}

static int list_empty(const struct list_head *head)
{
	return head->next == head;
}

#endif /* _LIST_H_ */
