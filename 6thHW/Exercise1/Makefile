CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: threadClient.o
				${CC} threadClient.o -o client

threadClient.o: threadClient.c
				${CC} ${CFLAGS} threadClient.c

server: threadServer.o
				${CC} -pthread threadServer.c -o server 

threadServer.o: threadServer.c
				${CC} ${CFLAGS} threadServer.c 

clean:
				rm -f *.o *~
