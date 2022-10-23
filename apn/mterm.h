// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct mterm {
	uint64_t ons;
	uint64_t dcs;
};

inline bool conforming(struct mterm *a, struct mterm *b)
{
	return (a->dcs == b->dcs) &&
	       (__builtin_popcountl(a->ons ^ b->ons) == 1);
}

inline struct mterm *merge(struct mterm *a, struct mterm *b)
{
	struct mterm *new_mterm;

	new_mterm = malloc(sizeof(struct mterm));
	new_mterm->ons = a->ons & b->ons;
	new_mterm->dcs = (a->ons ^ b->ons) | a->dcs;

	return new_mterm;
}
