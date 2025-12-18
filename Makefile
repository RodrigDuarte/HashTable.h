CFLAGS=-Wall -Wextra -pedantic -ggdb

.PHONY: ALL
all: example

example: example.c hashtable.h
	$(CC) $(CFLAGS) -o example example.c