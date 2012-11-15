OBJS := client_first_tcp.o
CC=gcc
LDFLAGS= -g -Wall
all: server_first_tcp client_first_tcp

clean:
		@rm -f *.o server_first_tcp client_first_tcp
