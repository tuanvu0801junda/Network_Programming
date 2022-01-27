CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: client server

client: client_select.o
				${CC} client_select.o -o client

client_select.o: client_select.c
				${CC} ${CFLAGS} client_select.c

server: server_select.o
				${CC} server_select.c -o server

server_select.o: server_select.c
				${CC} ${CFLAGS} server_select.c

clean:
				rm -f *.o *~
