CC = gcc
CFLAGS = -g -Wall -Wpointer-arith

out: malloc

malloc: main.c malloc.o
	$(CC) $(CFLAGS) $? -o $@

malloc.o: src/malloc.c
	$(CC) -c -g src/malloc.c

clean:
	rm -f *.o malloc
