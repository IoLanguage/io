CC=gcc
IO_CFLAGS=-I../../IoVM/_include
IO_LDFLAGS=-L../../IoVM/_libs -lIoVM -lm
CFLAGS=-O2 -g -Wall -pedantic $(IO_CFLAGS) $(IO_LDFLAGS) -ledit -ltermcap

all: clean iofe

iofe:
	$(CC) -o bin/iofe src/iofe.c $(CFLAGS)

clean:
	rm -f bin/iofe
