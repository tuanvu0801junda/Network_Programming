CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: exercise1

exercise1: main.o HazChemCode.o
	${CC} main.o HazChemCode.o -o exercise1

main.o: main.c
	${CC} ${CFLAGS} main.c

HarChemCode.o: HazChemCode.c
	${CC} ${CFLAGS} HazChemCode.c

clean:
	rm -f *.o *~


