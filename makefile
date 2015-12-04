CC=gcc
CFLAGS=
LDFLAGS=
EXEC=election
OBJ=$(wildcard *.o)

all: $(EXEC)

election: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
