// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "sort.h"
#include "mterm.h"

#define MAX(x, y) ((x > y) ? x : y)

static struct mterm **mterms;
static int mterms_cnt = 0;
static bool *merged;

static struct mterm **mterms2;
static int mterms_cnt2 = 0;

static int fsize, ones_cnt;
static uint64_t *ones;
static bool *in_result;

static bool cmp(uint64_t a, uint64_t b)
{
	struct mterm *ma = (struct mterm*) a;
	struct mterm *mb = (struct mterm*) b;

	/* Najpierw sortujemy po liczbie
	 * dc-ów. */
	if (__builtin_popcountl(ma->dcs) <
	    __builtin_popcountl(mb->dcs))
		return 1;

	if (__builtin_popcountl(ma->dcs) >
	    __builtin_popcountl(mb->dcs))
		return 0;

	/* Jeżeli minitermy mają taką
	 * samą liczbę dc-ów sortujemy
	 * po ich pozycjach. */
	if (ma->dcs < mb->dcs)
		return 1;

	if (ma->dcs > mb->dcs)
		return 0;

	/* Jeżeli minitermy mają takie
	 * same dc-y sortujemy po liczbie
	 * jedynek */
	if (__builtin_popcountl(ma->ons) <
	    __builtin_popcountl(mb->ons))
		return 1;

	return 0;
}

static void merge_segments(int l1, int h1, int l2, int h2)
{
	int i, j;

	for (i = l1; i < h1; i++) {
		for (j = l2; j < h2; j++) {
			if (conforming(mterms[i], mterms[j])) {
				mterms2[mterms_cnt2++] =
					merge(mterms[i], mterms[j]);
				
				merged[i] = 1;
				merged[j] = 1;
			}
		}
	}
}

static bool mak_step(void)
{
	int l1, l2, h1, h2;
	int cdcs, cons;
	struct mterm *prev;
	int i, j;
	int retval = 0;

	/* Zerujemy tablicę merged. */
	memset(merged, 0, mterms_cnt * sizeof(merged[0]));

	mterms_cnt2 = 0;
	l1 = h1 = l2 = h2 = 0;
	while (l1 < mterms_cnt) {
		cons = __builtin_popcountl(mterms[h1]->ons);
		cdcs = __builtin_popcountl(mterms[h1]->dcs);

		/* Znajdujemy zakres l1 - h1. */
		while (h1 < mterms_cnt &&
		       __builtin_popcountl(mterms[h1]->ons) == cons &&
		       __builtin_popcountl(mterms[h1]->dcs) == cdcs)
			h1++;
		
		/* Jeśli przejrzeliśmy całą tablicę i
		 * nie ma już z czym sklejać tej grupy
		 * minitermów. */
		if (h1 == mterms_cnt) break;

		/* Liczba dc-ów się różni, nie
		 * próbujemy skleić tych zakresów. */
		if (__builtin_popcountl(mterms[h1]->dcs) != cdcs) {
			cons = __builtin_popcountl(mterms[h1]->ons);
			cdcs = __builtin_popcountl(mterms[h1]->dcs);
			l1 = h1;

			continue;
		}

		l2 = h1;
		h2 = h1;
		cons = __builtin_popcountl(mterms[h2]->ons);
		
		/* Znajdujemy zakres l2 - h2. */
		while (h2 < mterms_cnt &&
		       __builtin_popcountl(mterms[h2]->ons) == cons &&
		       __builtin_popcountl(mterms[h2]->dcs) == cdcs)
			h2++;

		merge_segments(l1, h1, l2, h2);
		l1 = h1;
	}

	/* Kopiujemy minitermy, których
	 * nie udało nam się skleić z
	 * niczym. Minitermy już sklejone
	 * są niepotrzebne i możemy zwolnić
	 * pamięć którą zajmują. */
	for (i = 0; i < mterms_cnt; i++) {
		if (!merged[i]) {
			mterms2[mterms_cnt2++] = mterms[i];
		} else {
			free(mterms[i]);
			retval = 1;
		}
	}

	/* Sortujemy tablicę mterms2. */
	sort((uint64_t*) mterms2, mterms_cnt2, cmp);

	/* Kopiujemy zawartość tablicy
	 * mterms2 do mterms, ale tak,
	 * że nie kopiujemy powtarzających
	 * się elementów. Po posortowaniu
	 * tablicy, takie elementy powinny
	 * być obok siebie. */
	prev = mterms2[0];	
	i = 0, j = 0;
	while (j < mterms_cnt2) {
		while (j < mterms_cnt2 &&
		       mterms2[j]->ons == prev->ons &&
		       mterms2[j]->dcs == prev->dcs)
			j++;
		
		mterms[i] = prev;
		i++;

		if (j == mterms_cnt2) break;

		prev = mterms2[j];
	}

	/* i to teraz rozmiar naszej tablicy. */
	mterms_cnt = i;

	return retval;
}

static void print_miniterms(void)
{
	int i, j;

	for (i = 0; i < mterms_cnt; i++) {
		for (j = fsize - 1; j >= 0; j--) {
			if (mterms[i]->ons & (1 << j)) {
				printf("1");
				continue;
			}
			printf(mterms[i]->dcs & (1 << j) ? "-" : "0");
		}

		printf("\n");
	}
}

static void print_result(void)
{
	int i, j;

	for (i = 0; i < mterms_cnt; i++) {
		if (!in_result[i]) continue;

		for (j = fsize - 1; j >= 0; j--) {
			if (mterms[i]->ons & (1 << j)) {
				printf("1");
				continue;
			}
			printf(mterms[i]->dcs & (1 << j) ? "-" : "0");
		}

		printf("\n");
	}
}

int pot(int a, int b)
{
	int w;

	if (!b) return 1;

	w = pot(a, b >> 1);
	if (b & 1)
		return w * w * a;
	else
		return w * w;
}

int upbnd(int fsize, int ones_cnt)
{
	return ones_cnt * pot(fsize, fsize / 2);
}

int mak(int vcnt, int ocnt, uint64_t *tab, bool verbose)
{
	int slcted;
	int i, j;

	/* Liczba zmiennych */
	fsize = vcnt;

	/* Liczba jedynek */
	ones_cnt = ocnt;

	mterms_cnt = ones_cnt;
	ones = tab;
	
	/* upbnd() to szacowanie liczby
	 * minitermów w najgorszym przypadku. */
	mterms = malloc(upbnd(fsize, ones_cnt) * sizeof(mterms[0]));
	mterms2 = malloc(upbnd(fsize, ones_cnt) * sizeof(mterms2[0]));
	merged = malloc(upbnd(fsize, ones_cnt) * sizeof(merged[0]));

	for (i = 0; i < mterms_cnt; i++) {
		mterms[i] = malloc(sizeof(struct mterm));
		mterms[i]->ons = ones[i];
		mterms[i]->dcs = 0;
	}

	sort((uint64_t*) mterms, mterms_cnt, cmp);
	
	/* Wykonujemy nasz algorytm
	 * dopóki jesteśmy w stanie
	 * skleić co najmniej jedną
	 * parę. */
	while (mak_step()) {
		if (verbose) {
			printf("======\n");
			print_miniterms();
		}
	}

	/* Zwalniamy tablicę mterms2
	 * bo nie jest już potrzebna.
	 * Tablicę merged wykorzystamy
	 * jako in_result. */
	free(mterms2);
	in_result = merged;
	memset(in_result, 0, mterms_cnt * sizeof(in_result[0]));

	/* Wybieramy zasadnicze proste
	 * implikanty. Robimy to na
	 * pałę bo program i tak ma
	 * znacznie gorszą złożoność. */
	for (i = 0; i < ones_cnt; i++) {
		slcted = -1;
		for (j = 0; j < mterms_cnt; j++) {
			if (((mterms[j]->ons ^ ones[i]) &
			     ~mterms[j]->dcs) == 0) {
				if (slcted != -1) {
					slcted = -1;
					break;
				}
				
				slcted = j;
			}
		}

		if (slcted != -1)
			in_result[slcted] = 1;
	}

	if (verbose)
		printf("\nZasadnicze proste implikanty:\n");
	print_result();

	for (i = 0; i < mterms_cnt; i++)
		free(mterms[i]);

	free(mterms);
	free(merged);

	return(0);
}
