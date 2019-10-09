all:
	as add.s -o add.o
	gcc main.c -c -o main.o
	gcc main.o add.o -o prog

disassemble:
	gcc -g -O -Wa,-aslh main.c | less
