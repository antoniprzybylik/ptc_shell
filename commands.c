// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eng.h"
#include "apn/mak.h"
#include "help_msgs.h"
#include "msgs.h"
#include "main.h"

#define MAX(x, y) ((x > y) ? x : y)
#define MIN(x, y) ((x < y) ? x : y)

extern uint8_t VERBOSE;
#define VPRINT(...) do { if (VERBOSE) printf(__VA_ARGS__); } while (0)

extern struct fun **loaded_functions;
extern int funs_cnt;

int cmd_exit(int argc, char **argv)
{
	if (argc > 2) {
		printf("Exit: Za dużo argumentów.\n");
		return -1;
	}

	if (argc == 2)
		safe_exit(atoi(argv[1]));
	
	safe_exit(0);
}

static const char help_default_msg[] = ""
"\033[31;1mKomendy:\033[0m\n"
"exit\n"
"help\n"
"list\n"
"new\n"
"apn\n"
"kmap\n"
"cp\n"
"or\n"
"and\n"
"neg\n"
"fill\n"
"fill2\n"
"shrink\n"
"show\n";

int cmd_help(int argc, char **argv)
{
	struct msg *msg;

	if (argc == 1) {
		printf("%s", help_default_msg);
		return(0);
	}

	if (argc > 2) {
		printf("Help: Za dużo argumentów.\n");
		return -1;
	}

	msg = in_msg_set(argv[1], strlen(argv[1]));
	if (!msg) {
		printf("Help: Nie ma takiej komendy: \"%s\".\n", argv[1]);
		return -1;
	}

	printf("%s", msg->text);

	return(0);
}

int cmd_list(int argc, char **argv)
{
	int i;
	char buf[100];
	struct fun *fun;

	if (argc > 1) {
		printf("List: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		fun = loaded_functions[i];

		strftime(buf, sizeof(buf), "%D %T", gmtime(&fun->ts->tv_sec));
		printf("funkcja \"%s\": stworzona %s.%09ld UTC | %d zmiennych | %d jedynek\n",
		       fun->name, buf, fun->ts->tv_nsec, fun->vars_cnt, fun->ones_cnt);
	}

	if (i == 0)
		printf("=== NIE ZADEKLAROWANO FUNKCJI ===\n");

	return(0);
}

int cmd_new(int argc, char **argv)
{
	struct fun *fun;
	int i;

	if (argc == 1) {
		printf("New: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("New: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		fun = loaded_functions[i];

		if (!strcmp(fun->name, argv[1])) {
			printf("New: Błąd: Funkcja o nazwie \"%s\" istnieje!\n",
			       argv[1]);
			return -1;
		}
	}

	i = 0;
	while (argv[1][i]) {
		if (!((argv[1][i] >= 'a' && argv[1][i] <= 'z') ||
		     (argv[1][i] >= 'A' && argv[1][i] <= 'Z') ||
		     (argv[1][i] >= '0' && argv[1][i] <= '9'))) {
			printf("New: Błąd: Znak '%c' nie jest dozwolony w nazwach funkcji.\n",
			       argv[1][i]);
			return -1;
		}
		i++;
	}

	if (i > 31) {
		printf("New: Błąd: Za długa nazwa.\n"
		       "Maksymalna długość nazwy: 31.\n");
		return -1;
	}
	
	fun = new_fun(argv[1]);
	
	if (!funs_cnt)
		loaded_functions = malloc(sizeof(struct fun*));
	else
		loaded_functions = realloc(loaded_functions, funs_cnt + 1);

	loaded_functions[funs_cnt] = fun;
	funs_cnt++;

	return(0);
}

int cmd_mak(int argc, char **argv)
{
	int i;
	struct fun *fun = 0;

	if (argc == 1) {
		printf("Mak: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("Mak: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1])) {
			fun = loaded_functions[i];
			break;
		}
	}

	if (!fun) {
		printf("Mak: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	return mak(fun->vars_cnt, fun->ones_cnt, fun->ones, 1);
}

int cmd_apn(int argc, char **argv)
{
	int i;
	struct fun *fun = 0;

	if (argc == 1) {
		printf("Apn: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("Apn: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1])) {
			fun = loaded_functions[i];
			break;
		}
	}

	if (!fun) {
		printf("Apn: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	return mak(fun->vars_cnt, fun->ones_cnt, fun->ones, 0);
}

int cmd_kmap(int argc, char **argv)
{
	printf("Nie zaimplementowano.\n");
	return(0);
}

int cmd_cp(int argc, char **argv)
{
	int i;
	struct fun *fun1 = 0;
	struct fun *fun2 = 0;

	if (argc < 3) {
		printf("Cp: Musisz podać nazwy dwóch funkcji.\n");
		return -1;
	}

	if (argc > 3) {
		printf("Cp: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1]))
			fun1 = loaded_functions[i];
		if (!strcmp(loaded_functions[i]->name, argv[2]))
			fun2 = loaded_functions[i];
	}

	if (fun1 == fun2) {
		printf("Cp: Podałeś dwa razy nazwę tej samej funkcji.\n");
		return -1;
	}

	if (!fun1) {
		printf("Cp: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	if (!fun2) {
		printf("Cp: Nie ma takiej funkcji: \"%s\"\n", argv[2]);
		return -1;
	}
	
	fun2->ones_cnt = fun1->ones_cnt;
	fun2->vars_cnt = fun1->vars_cnt;
	fun2->ones = realloc(fun2->ones, fun1->ones_cnt * sizeof(fun1->ones[0]));
	memcpy(fun2->ones, fun1->ones, fun1->ones_cnt * sizeof(fun1->ones[0]));

	return(0);
}

int cmd_or(int argc, char **argv)
{
	int i;
	struct fun *fun1 = 0;
	struct fun *fun2 = 0;

	if (argc < 3) {
		printf("Or: Musisz podać nazwy dwóch funkcji.\n");
		return -1;
	}

	if (argc > 3) {
		printf("Or: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1]))
			fun1 = loaded_functions[i];
		if (!strcmp(loaded_functions[i]->name, argv[2]))
			fun2 = loaded_functions[i];
	}

	if (fun1 == fun2) {
		printf("Or: Podałeś dwa razy nazwę tej samej funkcji.\n");
		return -1;
	}

	if (!fun1) {
		printf("Or: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	if (!fun2) {
		printf("Or: Nie ma takiej funkcji: \"%s\"\n", argv[2]);
		return -1;
	}
	
	fun2->ones = realloc(fun2->ones, (fun1->ones_cnt + fun2->ones_cnt) * sizeof(fun1->ones[0]));
	memcpy(fun2->ones + fun2->ones_cnt, fun1->ones, fun1->ones_cnt * sizeof(fun1->ones[0]));
	fun2->ones_cnt += fun1->ones_cnt;
	fun2->vars_cnt = MAX(fun1->vars_cnt, fun2->vars_cnt);
	
	/* Normujemy fun2. */
	normalize_fun(fun2);

	return(0);
}

int cmd_and(int argc, char **argv)
{
	int i, j, k;
	struct fun *fun1 = 0;
	struct fun *fun2 = 0;

	int no_cnt = 0;
	uint64_t *no;

	if (argc < 3) {
		printf("And: Musisz podać nazwy dwóch funkcji.\n");
		return -1;
	}

	if (argc > 3) {
		printf("And: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1]))
			fun1 = loaded_functions[i];
		if (!strcmp(loaded_functions[i]->name, argv[2]))
			fun2 = loaded_functions[i];
	}

	if (fun1 == fun2) {
		printf("And: Podałeś dwa razy nazwę tej samej funkcji.\n");
		return -1;
	}

	if (!fun1) {
		printf("And: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	if (!fun2) {
		printf("And: Nie ma takiej funkcji: \"%s\"\n", argv[2]);
		return -1;
	}
	
	no_cnt = MIN(fun1->ones_cnt, fun2->ones_cnt);
	no = malloc(no_cnt * sizeof(no[0]));

	i = 0;
	j = 0;
	k = 0;

	while (i < fun1->ones_cnt &&
	       j < fun2->ones_cnt) {
		while (j < fun2->ones_cnt &&
		       fun1->ones[i] > fun2->ones[j]) j++;
		while (i < fun1->ones_cnt &&
		       fun1->ones[i] < fun2->ones[j]) i++;

		if (i >= fun1->ones_cnt ||
		    j >= fun2->ones_cnt)
			break;

		if (fun1->ones[i] == fun2->ones[j])
			no[k++] = fun1->ones[i];

		j++;
	}

	no_cnt = k;
	no = realloc(no, no_cnt * sizeof(no[0]));
	
	free(fun2->ones);
	fun2->ones_cnt = no_cnt;
	fun2->ones = no;

	fun2->vars_cnt = MAX(fun2->vars_cnt, fun1->vars_cnt);
	
	return(0);
}

int cmd_neg(int argc, char **argv)
{
	int i;
	struct fun *fun = 0;
	bool *is_one;

	uint64_t *no;
	int no_cnt = 0;

	if (argc == 1) {
		printf("Neg: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("Neg: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1])) {
			fun = loaded_functions[i];
			break;
		}
	}

	if (!fun) {
		printf("Neg: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	if (fun->vars_cnt > 16) {
		printf("Neg: Zbyt duża liczba zmiennych.\n");
		return -1;
	}

	is_one = calloc(sizeof(is_one[0]), (1 << fun->vars_cnt));
	no = malloc((1 << fun->vars_cnt) * sizeof(no[0]));

	for (i = 0; i < fun->ones_cnt; i++)
		is_one[fun->ones[i]] = 1;

	for (i = 0; i < (1 << fun->vars_cnt); i++) {
		if (!is_one[i])
			no[no_cnt++] = i;
	}

	free(is_one);
	free(fun->ones);

	no = realloc(no, no_cnt * sizeof(no[0]));

	fun->ones = no;
	fun->ones_cnt = no_cnt;

	return(0);
}

int cmd_fill(int argc, char **argv)
{
	int i;
	char c;
	int trm;
	struct fun *fun = 0;
	int ones_cnt;

	if (argc == 1) {
		printf("Fill: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("Fill: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1])) {
			fun = loaded_functions[i];
			break;
		}
	}

	if (!fun) {
		printf("Fill: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	if (!fun->ones)
		fun->ones = malloc(sizeof(fun->ones[0]));
	trm = 1;

	printf("Wprowadź jedynki: ");

	/* Za każdym razem kiedy potrzebujemy
	 * pamięci rezerwujemy dwa razy więcej
	 * niż już zarezerwowaliśmy żeby nie
	 * robić tego za dużo razy. */
	ones_cnt = 0;
	do {
		if (ones_cnt == trm)
			trm *= 2;
			
		if (trm > fun->ones_cnt) {
			fun->ones = realloc(fun->ones, trm * sizeof(fun->ones[0]));
			fun->ones_cnt = trm;
		}

		scanf("%lu%c", &fun->ones[ones_cnt], &c);
		ones_cnt++;
	} while (c && c != '\n');

	if (ones_cnt > 100000) {
		printf("Fill: Maksymalna liczba jedynek to 100000.\n");
		free(fun->ones);
		fun->ones_cnt = 0;
		fun->vars_cnt = 0;
		return -1;
	}

	/* Na koniec zwalniamy nadwyżkę. */
	fun->ones = realloc(fun->ones, ones_cnt * sizeof(fun->ones[0]));
	fun->ones_cnt = ones_cnt;

	fun->vars_cnt = 0;
	for (i = 0; i < ones_cnt; i++) {
		if (!fun->ones[i]) continue;
		fun->vars_cnt = MAX(fun->vars_cnt,
				    64 - __builtin_clzl(fun->ones[i]));
	}

	/* Normujemy funkcję. */
	normalize_fun(fun);

	return(0);
}

int cmd_fill2(int argc, char **argv)
{
	int i;
	char c;
	int trm;
	struct fun *fun = 0;
	int ones_cnt;
	int vars_cnt;

	if (argc == 1) {
		printf("Fill2: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("Fill2: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1])) {
			fun = loaded_functions[i];
			break;
		}
	}

	if (!fun) {
		printf("Fill2: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	if (!fun->ones)
		fun->ones = malloc(sizeof(fun->ones[0]));
	trm = 1;

	printf("Liczba zmiennych: ");
	scanf("%d", &vars_cnt);

	printf("Wprowadź jedynki: ");

	/* Za każdym razem kiedy potrzebujemy
	 * pamięci rezerwujemy dwa razy więcej
	 * niż już zarezerwowaliśmy żeby nie
	 * robić tego za dużo razy. */
	ones_cnt = 0;
	do {
		if (ones_cnt == trm)
			trm *= 2;
			
		if (trm > fun->ones_cnt) {
			fun->ones = realloc(fun->ones, trm * sizeof(fun->ones[0]));
			fun->ones_cnt = trm;
		}

		scanf("%lu%c", &fun->ones[ones_cnt], &c);
		ones_cnt++;

		if (fun->ones[ones_cnt] &&
		    (64 - __builtin_clzl(fun->ones[ones_cnt])) > vars_cnt) {
			printf("Fill2: Błąd: W jedynce %lu jest więcej zmiennych niż zadeklarowano!\n",
			       fun->ones[ones_cnt]);
			free(fun->ones);
			fun->ones = 0;

			return -1;
		}
	} while (c && c != '\n');

	if (ones_cnt > 100000) {
		printf("Fill2: Maksymalna liczba jedynek to 100000.\n");
		free(fun->ones);
		fun->ones_cnt = 0;
		fun->vars_cnt = 0;
		return -1;
	}

	/* Na koniec zwalniamy nadwyżkę. */
	fun->ones = realloc(fun->ones, ones_cnt * sizeof(fun->ones[0]));
	fun->ones_cnt = ones_cnt;

	fun->vars_cnt = vars_cnt;

	/* Normujemy funkcję. */
	normalize_fun(fun);

	return(0);
}

int cmd_shrink(int argc, char **argv)
{
	int i;
	struct fun *fun = 0;

	if (argc == 1) {
		printf("Shrink: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("Shrink: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1])) {
			fun = loaded_functions[i];
			break;
		}
	}

	if (!fun) {
		printf("Shrink: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}
	
	if (!fun->ones) {
		printf("Shrink: Funkcja nie jest wypełniona!\n");
		return -1;
	}

	fun->vars_cnt = 0;
	for (i = 0; i < fun->ones_cnt; i++) {
		if (!fun->ones[i]) continue;
		fun->vars_cnt = MAX(fun->vars_cnt,
				    64 - __builtin_clzl(fun->ones[i]));
	}

	return(0);
}

int cmd_show(int argc, char **argv)
{
	int i;
	struct fun *fun = 0;

	if (argc == 1) {
		printf("Show: Musisz podać nazwę funkcji.\n");
		return -1;
	}

	if (argc > 2) {
		printf("Show: Za dużo argumentów.\n");
		return -1;
	}

	for (i = 0; i < funs_cnt; i++) {
		if (!strcmp(loaded_functions[i]->name, argv[1])) {
			fun = loaded_functions[i];
			break;
		}
	}

	if (!fun) {
		printf("Show: Nie ma takiej funkcji: \"%s\"\n", argv[1]);
		return -1;
	}

	if (!fun->ones) {
		printf("Show: Funkcja nie jest wypełniona!\n");
		return -1;
	}

	for (i = 0; i < fun->ones_cnt; i++)
		printf("%lu ", fun->ones[i]);
	printf("\n");

	return(0);
}
