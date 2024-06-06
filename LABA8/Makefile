CC = gcc
CFLAGS_RELEASE = -std=c11 -pedantic -W -Wall -Wextra

all: client server

client: client.c
	$(CC) $(CFLAGS_RELEASE) $^ -o $@

server: server.c
	$(CC) $(CFLAGS_RELEASE) $^ -o $@ 

clean:
	rm -f server client