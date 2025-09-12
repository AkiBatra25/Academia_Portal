# Makefile

CC = gcc
CFLAGS = -Iinclude

all: server client init

server: server/server.c server/common.c
	$(CC) $(CFLAGS) -o server/server server/server.c server/common.c -lpthread

client: client/client.c
	$(CC) $(CFLAGS) -o client/client client/client.c

init: init/init_data.c
	$(CC) $(CFLAGS) -o init/init_data init/init_data.c

clean:
	rm -f server/server client/client init/init_data
	rm -f server/*.dat
