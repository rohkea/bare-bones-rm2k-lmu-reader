CC=gcc

all: mapread

mapread.o: mapread.c
	$(CC) -c -o mapread.o mapread.c

example.o: example.c
	$(CC) -c -o example.o example.c

mapread: example.o mapread.o
	$(CC) -o mapread example.o mapread.o

clean:
	rm -f mapread.o example.o mapread

.PHONY: clean all
