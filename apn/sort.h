// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdint.h>
#include <stdbool.h>

struct node {
	uint64_t val;
	struct node *next;
};

struct qnode {
	struct node *e;
	struct qnode *next;
};

struct queue {
	struct qnode *first;
	struct qnode *last;
};

struct qnode *init_qlist(struct node *e);
void free_qlist(struct qnode *e);
struct qnode *insert_qlist(struct qnode *prev, struct node *e);
struct node *init_list(uint64_t val);
void free_list(struct node *e);
struct node *insert(struct node *prev, uint64_t val);
void sort(uint64_t *tab, int size, bool (*cmp)(uint64_t, uint64_t));
