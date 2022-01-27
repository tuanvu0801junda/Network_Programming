CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: resolver

resolver: resolver.o util.o
		${CC} resolver.o util.o -o resolver

resolver.o: resolver.c
		${CC} ${CFLAGS} resolver.c

util.o: util.c
		${CC} ${CFLAGS} util.c
clean:
		rm -f *.o *~