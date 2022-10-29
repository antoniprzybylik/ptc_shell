// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdint.h>
#include <stdbool.h>

void sort(uint64_t *tab, int size, bool (*cmp)(uint64_t, uint64_t));
