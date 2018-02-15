.PHONY: all check clean

all: fb64d.o

fb64d.o: fb64d.c fb64d.h
	gcc -Wall -shared -O3 -c $<

test: test.c fb64d.o
	gcc -Wall -O3 -o $@ $^

example: example.c fb64d.o
	gcc -Wall -o $@ $^

check: example test
	./example > /dev/null
	./test

clean:
	rm -f fb64d.o test example
