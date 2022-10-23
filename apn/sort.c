// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "sort.h"

struct qnode *init_qlist(struct node *e)
{
	struct qnode *current;

	current = malloc(sizeof(struct qnode));
	current->e = e;
	current->next = 0;
	
	return current;
}

void free_qlist(struct qnode *e)
{
	struct qnode *prev;

	while (e) {
		prev = e;
		e = e->next;
		free_list(prev->e);
		free(prev);
	}
}

struct qnode *insert_qlist(struct qnode *prev, struct node *e)
{
	struct qnode *current;

	current = malloc(sizeof(struct qnode));
	current->e = e;
	current->next = prev->next;
	prev->next = current;

	return current;
}

struct node *init_list(uint64_t val)
{
	struct node *current;

	current = malloc(sizeof(struct node));
	current->val = val;
	current->next = 0;

	return current;
}

void free_list(struct node *e)
{
	struct node *prev;

	while (e) {
		prev = e;
		e = e->next;
		free(prev);
	}
}

struct node *insert(struct node *prev, uint64_t	val)
{
	struct node *current;

	current = malloc(sizeof(struct node));
	current->val = val;
	current->next = prev->next;
	prev->next = current;

	return current;
}

inline bool eval1(struct node *a, struct node *b, bool (*cmp)(uint64_t, uint64_t))
{
	if (!a) return false;
	if (!b) return true;
	return cmp(a->val, b->val);

	return false;
}

inline bool eval2(struct node *a, struct node *b, bool (*cmp)(uint64_t, uint64_t))
{
	if (!b) return false;
	if (!a) return true;
	return !cmp(a->val, b->val);

	return false;
}

struct node *sorted_merge(struct node *a, struct node *b, bool (*cmp)(uint64_t, uint64_t))
{
	struct node *result, *fresult;

	result = init_list(0);
	result->next = result;

	while (a || b) {
		while (eval1(a, b, cmp)) {
			result = insert(result, a->val);
			a = a->next;
		}

		while (eval2(a, b, cmp)) {
			result = insert(result, b->val);
			b = b->next;
		}
	}

	fresult = result->next->next;
	result->next = 0;

	return fresult;
}

void sort(uint64_t *tab, int size, bool (*cmp)(uint64_t, uint64_t))
{
	struct node *list;
	struct queue *q;
	struct qnode *qb;
	struct node *a, *b;
	int i;

	if (size < 2) return;

	q = malloc(sizeof(struct queue));
	q->first = init_qlist(init_list(tab[0]));
	q->last = q->first;
	qb = q->first;

	for (i = 1; i < size; i++)
		q->first = insert_qlist(q->first, init_list(tab[i]));

	while (q->last) {
		a = q->last->e;
		q->last = q->last->next;
		b = q->last->e;
		q->last = q->last->next;
		q->first = insert_qlist(q->first, sorted_merge(a, b, cmp));
	}

	list = q->first->e;

	i = 0;
	while (list) {
		tab[i] = list->val;
		list = list->next;
		i++;
	}

	free_qlist(qb);
}
