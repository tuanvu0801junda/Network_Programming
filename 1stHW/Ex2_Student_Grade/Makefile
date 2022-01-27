CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: exercise2

exercise2: main.o support.o structure.o
	${CC} main.o support.o structure.o -o exercise2

main.o: main.c
	${CC} ${CFLAGS} main.c

support.o: support.c
	${CC} ${CFLAGS} support.c

structure.o: structure.c
	${CC} ${CFLAGS} structure.c
	
clean:
	rm -f *.o *~


