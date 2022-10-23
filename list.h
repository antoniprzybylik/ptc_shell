// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdlib.h>

struct list {
	void *content;
	struct list *next;
};

static inline struct list *list_create(void *content)
{
	struct list *first_elem;

	first_elem = malloc(sizeof(struct list));
	first_elem->content = content;
	first_elem->next = 0;

	return first_elem;
}

static inline struct list *list_add(struct list *list, void *content)
{
	struct list *new_elem;

	new_elem = malloc(sizeof(struct list));
	new_elem->content = content;
	new_elem->next = 0;

	list->next = new_elem;
	return new_elem;
}
