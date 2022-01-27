CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: client.o 
				${CC} client.o -o client

client.o: client.c
				${CC} ${CFLAGS} client.c

server: server.o util.o
				${CC} server.o util.o -o server

server.o: server.c
				${CC} ${CFLAGS} server.c

util.o: util.c
				${CC} ${CFLAGS} util.c

clean:
				rm -f *.o *~