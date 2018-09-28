CC = gcc
CFLAGS = -g -Wall -Wpointer-arith

all: malloc free

malloc: malloc.o
	$(CC) $(CFLAGS) $? -lm -o $@

free: free.o
	$(CC) $(CFLAGS) $? -lm -o $@

malloc.o: src/malloc.c src/malloc.h
	$(CC) -c -g src/malloc.c

free.o: src/free.c src/free.h
	$(CC) -c -g src/free.c

clean:
	rm *.o malloc free
