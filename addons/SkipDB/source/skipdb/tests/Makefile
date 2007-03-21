OS ?= $(shell uname -s)

CFLAGS := -I../_build/headers -I../../basekit/_build/headers
LFLAGS := -L../_build/lib -lskipdb -L../../basekit/_build/lib -lbasekit

NEEDS_M := FreeBSD Linux NetBSD
ifneq (,$(findstring $(OS),$(NEEDS_M)))
LFLAGS +=-lm
endif

all:
	$(CC) $(CFLAGS) skipdbtest.c -o skipdbtest $(LFLAGS)

clean:
	-rm skipdbtest

test:
	./skipdbtest
