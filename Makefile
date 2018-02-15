.PHONY: all check clean

all: fb64d.o

fb64d.o: fb64d.c
	gcc -Wall -shared -O3 -c $^

test: test.c fb64d.o
	gcc -Wall -O3 -o $@ $^

check: test
	./test

clean:
	rm -f fb64d.o test
