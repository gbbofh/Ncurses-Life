#all: test
all: main

#test: test.c
#	gcc -std=c99 -o test -g -Wall test.c -lncursesw

main: main.c life.c life.h
	gcc -std=c99 -o life main.c life.c -g -Wall -lncursesw -lrt -lm

.PHONY:

clean:
	rm life
