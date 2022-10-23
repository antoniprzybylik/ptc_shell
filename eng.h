// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdint.h>
#include <time.h>

#include "list.h"

struct fun {
	uint64_t magic;
	struct timespec *ts;

	char *name;
	int vars_cnt;
	int ones_cnt;
	uint64_t *ones;
};

struct fun *new_fun(char *name);
void normalize_fun(struct fun *fun);
