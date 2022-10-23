// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/uio.h>

#include "eng.h"
#include "shell.h"
#include "apn/sort.h"

extern uint8_t VERBOSE;
#define VPRINT(...) do { if (VERBOSE) printf(__VA_ARGS__); } while (0)

static struct fun fun_template = {
	.magic = 0x7017D1395252A18D,
	.vars_cnt = 0,
	.ones_cnt = 0,
	.ones = 0,
};

struct fun *new_fun(char *name)
{
	struct fun *new_fun;
	struct timespec *ts;

	new_fun = malloc(sizeof(struct fun));
	*new_fun = fun_template;

	ts = malloc(sizeof(struct timespec));
	timespec_get(ts, 1); /* 1 = TIME_UTC */
	new_fun->ts = ts;

	new_fun->name = malloc(strlen(name) + 1);
	strcpy(new_fun->name, name);

	return new_fun;
}

static bool cmp(uint64_t a, uint64_t b)
{
	return a < b;
}

void normalize_fun(struct fun *fun)
{
	int i, j;
	uint64_t prev;

	if (fun->ones_cnt < 2) return;

	sort(fun->ones, fun->ones_cnt, cmp);

	j = 1;
	prev = fun->ones[0];
	for (i = 1; i < fun->ones_cnt; i++) {
		if (fun->ones[i] == prev) continue;
		fun->ones[j++] = fun->ones[i];
		prev = fun->ones[i];
	}

	fun->ones = realloc(fun->ones, j * sizeof(fun->ones[0]));
	fun->ones_cnt = j;
}
