.PHONY: all check clean

COMPILE_OBJ = gcc -Wall -shared -O3 -c

all: encode.o decode.o

%.o: %.c %.h common.h
	$(COMPILE_OBJ) $<

test: test.c decode.o
	gcc -Wall -O3 -o $@ $^

example: example.c decode.o
	gcc -Wall -o $@ $^

check: example test
	./example > /dev/null
	./test

clean:
	rm -f *.o test example
