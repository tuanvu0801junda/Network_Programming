CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: threadClient.o BST.o
		${CC} threadClient.o BST.o -o client

threadClient.o: threadClient.c
		${CC} ${CFLAGS} threadClient.c

server: threadServer.o BST.o
		${CC} -pthread BST.o threadServer.o -o server 

threadServer.o: threadServer.c
		${CC} ${CFLAGS} threadServer.c 

BST.o: BST.c
		${CC} ${CFLAGS} BST.c

clean:
		rm -f *.o *~