// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>

#include "eng.h"
#include "shell.h"

uint8_t VERBOSE = 0;
#define VPRINT(...) do { if (VERBOSE) printf(__VA_ARGS__); } while (0)

static struct termios termios;

void save_termios(void)
{
	tcgetattr(0, &termios);
}

void restore_termios(void)
{
	tcsetattr(0, TCSANOW, &termios);
}

void safe_exit(int code)
{
	/* Przywracamy termios. */
	restore_termios();

	/* Resetujemy ustawienia terminala. */
	write(0, "\033[0m", 4);

	_exit(code);
}

struct fun **loaded_functions = 0;
int funs_cnt = 0;

int main(int argc, char **argv)
{
	struct header *header;
	struct player *p1, *p2, *p3;
	struct list *last_player;

	int err;
	int i;

	/* Zapisujemy termios. */
	save_termios();

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-v") ||
		    !strcmp(argv[i], "--verbose")) {
			VERBOSE = 1;
		} else {
			fprintf(stderr, "Błąd: Nieznany argument \"%s\".\n", argv[i]);
			return -1;
		}
	}
	
	shell_main();
}
