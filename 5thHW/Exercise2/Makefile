CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: forkClient.o
				${CC} forkClient.o -o client

forkClient.o: forkClient.c
				${CC} ${CFLAGS} forkClient.c

server: forkServer.o
				${CC} forkServer.c -o server

forkServer.o: forkServer.c
				${CC} ${CFLAGS} forkServer.c

clean:
				rm -f *.o *~
