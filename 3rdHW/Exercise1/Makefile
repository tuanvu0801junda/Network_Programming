CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: client.o 
				${CC} client.o -o client

client.o: client.c
				${CC} ${CFLAGS} client.c

server: server.o
				${CC} server.o -o server

server.o: server.c
				${CC} ${CFLAGS} server.c

clean:
				rm -f *.o *~
