CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: client_nbio.o
				${CC} client_nbio.o -o client

client_nbio.o: client_nbio.c
				${CC} ${CFLAGS} client_nbio.c

server: server_nbio.o
				${CC} server_nbio.c -o server

server_nbio.o: server_nbio.c
				${CC} ${CFLAGS} server_nbio.c

clean:
				rm -f *.o *~
