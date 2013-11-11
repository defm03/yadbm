CFLAGS=-Wall -g
SRCDIR = src/

all:
	cc $(CFLAGS) $(SRCDIR)yadbm.c

clean:
	rm -f yadbm
