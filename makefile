CC=gcc
CFLAGS=-std=c99 -D_POSIX_SOURCE
LDFLAGS=
EXEC=election
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(EXEC)

election: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
