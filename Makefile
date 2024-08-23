CC=gcc -g
CFLAGS=-Wall -Wextra -std=c99

TARGETS=sfl

build: $(TARGETS)

run_sfl: sfl.o
	$(CC) $(CFLAGS) sfl.o -o sfl
sfl.o: sfl.c sfl.h
	$(CC) $(CFLAGS) -c sfl.c

clean:
	rm -f sfl sfl.o