.PHONY: all check clean runbench

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

bench: benchmark.cpp fb64d.o
	g++ -Wall -O3 -o $@ $^ -lbenchmark

runbench: bench
	./bench

clean:
	rm -f fb64d.o test example
