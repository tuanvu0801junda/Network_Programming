CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: client_signalio.o
				${CC} client_signalio.o -o client

client_signalio.o: client_signalio.c
				${CC} ${CFLAGS} client_signalio.c

server: server_signalio.o
				${CC} server_signalio.c -o server

server_signalio.o: server_signalio.c
				${CC} ${CFLAGS} server_signalio.c

clean:
				rm -f *.o *~