// SPDX-License-Identifier: GPL-2.0
// Author: Antoni Przybylik

static const char exit_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"exit\n"
"exit <kod wyjścia>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Opuszcza powłokę.\n";

static const char help_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"help\n"
"help <komenda>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Wyświetla pomoc.\n";

static const char new_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"new <funkcja>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Deklaruje nową funkcję.\n";

static const char list_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"list\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Wypisuje zadeklarowane funkcje\n"
"wraz z informacjami na ich temat.\n";

static const char apn_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"apn <funkcja>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Wyświetla funkcję w postaci\n"
"minimalnej APN.\n";

static const char mak_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"mak <funkcja>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Wyświetla kroki algorytmu\n"
"Quine'a i McCluskay'a (trochę\n"
"zmienionego przeze mnie).\n";

static const char kmap_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"kmap <funkcja>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Rysuje mapę Karnaugha.\n";

static const char cp_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"cp <funkcja1> <funkcja2>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Kopiuje funkcję 1 do funkcji 2.\n";

static const char or_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"or <funkcja1> <funkcja2>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Oblicza alternatywę funkcji i\n"
"zapisuje wynik w funkcji 2.\n";

static const char and_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"and <funkcja1> <funkcja2>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Oblicza koniunkcję funkcji i\n"
"zapisuje wynik w funkcji 2.\n";

static const char neg_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"neg <funkcja>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Neguje funkcję. Czyli nowa\n"
"funkcja będzie miała zera tam\n"
"gdzie ta ma jedynki.\n";

static const char fill_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"fill <funkcja>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Wypełnia jedynki funkcji.\n";

static const char show_msg[] = ""
"\033[31;1mSchematy:\033[0m\n"
"show <funkcja>\n"
"\n"
"\033[31;1mOpis:\033[0m\n"
"Wyświetla jedynki funkcji.\n";
