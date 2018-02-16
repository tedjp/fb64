.PHONY: all check clean

COMPILE_OBJ = gcc -Wall -shared -O3 -c

all: fb64d.o fb64e.o

%.o: %.c %.h
	$(COMPILE_OBJ) $<

test: test.c fb64d.o
	gcc -Wall -O3 -o $@ $^

example: example.c fb64d.o
	gcc -Wall -o $@ $^

check: example test
	./example > /dev/null
	./test

clean:
	rm -f fb64d.o test example
