.PHONY: all clean build debug

all: build

build:
	gperf -t -L C -N in_command_set cmds.gperf > cmds.h
	gperf -t -L C -N in_msg_set msgs.gperf > msgs.h

	gcc eng.c -o eng.o -c -O3 -std=gnu89
	gcc main.c -o main.o -c -O3 -std=gnu89 -Wno-variadic-macros
	gcc commands.c -o commands.o -c -O3 -std=gnu89 -Wno-variadic-macros
	gcc shell.c -o shell.o -c -O3 -std=gnu89 -Wno-variadic-macros

	gcc apn/mak.c -o apn/mak.o -c -O3 -std=gnu89
	gcc apn/sort.c -o apn/sort.o -c -O3 -std=gnu89
	ld apn/mak.o apn/sort.o -o apn/apn.o -r

	gcc main.o shell.o commands.o eng.o apn/apn.o -o ptc_shell -static
	strip ptc_shell

debug:
	gperf -t -L C -N in_command_set cmds.gperf > cmds.h
	gperf -t -L C -N in_msg_set msgs.gperf > msgs.h

	gcc eng.c -o eng.o -c -O0 -std=gnu89 -g
	gcc main.c -o main.o -c -O0 -std=gnu89 -Wno-variadic-macros -g
	gcc commands.c -o commands.o -c -O0 -std=gnu89 -Wno-variadic-macros -g
	gcc shell.c -o shell.o -c -O0 -std=gnu89 -Wno-variadic-macros -g

	gcc apn/mak.c -o apn/mak.o -c -O0 -std=gnu89 -g
	gcc apn/sort.c -o apn/sort.o -c -O0 -std=gnu89 -g
	ld apn/mak.o apn/sort.o -o apn/apn.o -r

	gcc main.o shell.o commands.o eng.o apn/apn.o -o ptc_shell -static

clean:
	rm *.o */*.o ptc_shell
