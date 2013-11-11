CFLAGS=-Wall -g
SRCDIR = src/

all:
	cc $(CFLAGS) $(SRCDIR)yadbm.c -o yadbm

clean:
	rm -f yadbm
