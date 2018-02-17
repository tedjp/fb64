.PHONY: all check clean

COMPILE_OBJ = gcc -Wall -shared -O3 -c

OBJS = common.o encode.o decode.o

all: $(OBJS)

%.o: %.c common.h
	$(COMPILE_OBJ) $<

test: test.c $(OBJS)
	gcc -Wall -O3 -o $@ $^

example: example.c $(OBJS)
	gcc -Wall -o $@ $^

check: example test
	./example > /dev/null
	./test

clean:
	rm -f *.o test example
