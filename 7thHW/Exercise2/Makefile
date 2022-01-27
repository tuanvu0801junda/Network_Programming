CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: pollClient.o BST.o
		${CC} pollClient.o BST.o -o client

pollClient.o: pollClient.c
		${CC} ${CFLAGS} pollClient.c

server: pollServer.o BST.o
		${CC} BST.o pollServer.o -o server 

pollServer.o: pollServer.c
		${CC} ${CFLAGS} pollServer.c 

BST.o: BST.c
		${CC} ${CFLAGS} BST.c

clean:
		rm -f *.o *~