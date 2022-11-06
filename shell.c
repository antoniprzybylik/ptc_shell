// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shell.h"
#include "cmds.h"

static const char msg[] = ""
"PTC Shell v1.0.0\n"
"\n"
"Napisz \"help\" żeby uzyskać pomoc, \"help <komenda>\"\n"
"żeby uzyskać pomoc dot. konkretnej komendy.\n"
"\n"
"\033[36m>>>\033[0m ";

extern uint8_t VERBOSE;
#define VPRINT(...) do { if (VERBOSE) printf(__VA_ARGS__); } while (0)

static void interpret(char *buf)
{
	uint8_t i, j, k;
	char cmd_name[64];
	struct command *command;
	int (*cmd)(int, char**);
	char *argv[32];
	int argc;

	struct termios termios;

	i = 0;
	while (buf[i] == ' ' || buf[i] == '\t') i++;

	/* Jeśli linia jest pusta. */
	if (buf[i] == '\0') return;

	/* ********** */
	/* ********** */

	fflush(0);

	/* Włączamy tryb kanoniczny. */
	restore_termios();

	/* ********** */
	/* ********** */

	j = 0;
	while (buf[i] != ' ' && buf[i] != '\t' && buf[i])
		cmd_name[j++] = buf[i++];
	cmd_name[j] = '\0';

	command = in_command_set(cmd_name, j);
	if (command == 0) {
		printf("\033[32m[shell]\033[0m \033[31mBłąd:\033[0m "
		       "Nie ma takiej komendy: \"%s\".\n", cmd_name);
		goto error;
	}
	cmd = command->method;

	argv[0] = malloc(strlen(cmd_name) + 1);
	strcpy(argv[0], cmd_name);
	argc = 1;
	while (i < 64) {
		while (buf[i] == ' ' || buf[i] == '\t') i++;
		if (!buf[i]) break;

		k = i;
		j = 0;
		while (buf[i] != ' ' && buf[i] != '\t' && buf[i]) i++, j++;
		argv[argc] = malloc(j + 1);
		memcpy(argv[argc], &buf[k], j);
		argv[argc][j] = '\0';
		argc++;
		if (!buf[i]) break;
	}

	VPRINT("\033[32m[shell]\033[0m Running command: \"%s\"...\n", cmd_name);
	cmd(argc, argv);
	VPRINT("\033[32m[shell]\033[0m Done.\n");

	/* ********** */
	/* ********** */

	fflush(0);
	
	/* Wyłączamy tryb kanoniczny. */
	tcgetattr(0, &termios);
	termios.c_lflag &= ~ICANON;
	termios.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &termios);

	/* ********** */
	/* ********** */

	return;

error:

	/* ********** */
	/* ********** */

	fflush(0);

	/* Wyłączamy tryb kanoniczny. */
	tcgetattr(0, &termios);
	termios.c_lflag &= ~ICANON;
	termios.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &termios);

	/* ********** */
	/* ********** */
}

static inline bool eol_check(uint8_t *i_ref, uint8_t *line_size_ref, char *buf, char pc)
{
	uint8_t i = *i_ref;
	uint8_t line_size = *line_size_ref;

	if (buf[i] == '\n' || i == 63) {
		if (buf[i] == '\n') {
			buf[i] = pc;
			buf[line_size] = '\0';
		} else {
			i++;
			line_size = i > line_size ?
				    i : line_size;
			buf[line_size] = '\0';
			write(0, "\n", 1);
		}
			
		interpret(buf);
		write(0, "\033[36m>>>\033[0m ", 13);
		i = 0;
		line_size = 0;
	} else {
		i++;
		line_size = i > line_size ?
			    i : line_size;
	}

	*i_ref = i;
	*line_size_ref = line_size;
	return (i != 0);
}

void shell_main(void)
{
	char buf[65];
	uint8_t ccnt[65];
	uint8_t i = 0;
	uint8_t j = 0;

	int bytes;
	int in_esc_seq = 0;
	uint8_t line_size = 0;
	char pc;

	struct termios termios;

	/* Wyłączamy tryb kanoniczny. */
	/* *** */
	tcgetattr(0, &termios);
	termios.c_lflag &= ~ICANON;
	termios.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &termios);
	/* *** */

	write(0, msg, sizeof(msg));

	do {
		pc = buf[i];
		bytes = read(1, &buf[i], 1);
		if (bytes != 1) break;

		if (in_esc_seq == 1) {
			if (buf[i] == '[') {
				in_esc_seq++;
				buf[i] = pc;
				continue;
			} else {
				in_esc_seq = 0;

				buf[i] == '?';
				write(0, "\033[91m?\033[0m", 10);
				if (!eol_check(&i, &line_size, buf, pc))
					continue;
			}
		}

		if (in_esc_seq == 2) {
			in_esc_seq = 0;

			switch (buf[i]) {
				case 'A':
					continue;
				case 'B':
					continue;
				case 'C':
					buf[i] = pc;
					if (i >= line_size)
						continue;

					i++;
					write(0, "\033[C", 3);
					continue;
				case 'D':
					buf[i] = pc;
					if (i <= 0)
						continue;

					i--;
					write(0, "\033[D", 3);
					continue;
				default:
					buf[i] == '?';
					write(0, "\033[91m?\033[0m", 10);
					if (!eol_check(&i, &line_size, buf, pc))
						continue;
					buf[i] == '?';
					write(0, "\033[91m?\033[0m", 10);
					if (!eol_check(&i, &line_size, buf, pc))
						continue;
			}
		}

		switch (buf[i]) {
		case '\177':
			if (i == 0) {
				i = -1;
				break;
			}

			buf[i] = pc;
			memcpy(&buf[i-1], &buf[i], line_size - i);
			i -= 1;
			line_size -= 1;

			write(0, "\b", 1);
			write(0, &buf[i], line_size - i);
			write(0, " \b", 2);

			for (j = i; j < line_size; j++)
				write(0, "\033[D", 3);

			continue;

		/* Klawisz escape. */
		case '\033':
			buf[i] = pc;
			in_esc_seq = 1;
			continue;

		default:
			if ((buf[i] >= 'a' && buf[i] <= 'z') ||
			    (buf[i] >= ' ' && buf[i] <= '?') ||
			    (buf[i] == '\n') ||
			    (buf[i] == '_')) {
				write(0, &buf[i], 1);
			} else {
				write(0, "\033[91m?\033[0m", 10);
				buf[i] = '?';
			}

		}

		eol_check(&i, &line_size, buf, pc);
	} while (bytes == 1);
}
