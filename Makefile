.PHONY: all check clean runbench install uninstall

STATIC_LIB = libfb64.a

COMPILE_OBJ = gcc -Wall -shared -O3 -fvisibility=hidden -fPIC -c

OBJS = encode.o decode.o

all: fb64 $(STATIC_LIB)

%.o: %.c
	$(COMPILE_OBJ) $<

install:
	mkdir -p -- $(DESTDIR)/usr/local/bin
	cp -- fb64 $(DESTDIR)/usr/local/bin
	mkdir -p -- $(DESTDIR)/usr/local/lib
	cp -- $(STATIC_LIB) $(DESTDIR)/usr/local/lib
	mkdir -p -- $(DESTDIR)/usr/local/include
	cp -- fb64.h $(DESTDIR)/usr/local/include

uninstall:
	rm -f -- $(DESTDIR)/usr/local/bin/fb64
	rmdir --ignore-fail-on-non-empty -- $(DESTDIR)/usr/local/bin
	rm -f -- $(DESTDIR)/usr/local/lib/$(STATIC_LIB)
	rmdir --ignore-fail-on-non-empty -- $(DESTDIR)/usr/local/lib
	rm -f -- $(DESTDIR)/usr/local/include/fb64.h
	rmdir --ignore-fail-on-non-empty -- $(DESTDIR)/usr/local/include

$(STATIC_LIB): $(OBJS)
	ar rcs $@ $^

test: test.c $(OBJS)
	gcc -Wall -O3 -o $@ $^

example: example.c $(OBJS)
	gcc -Wall -o $@ $^

check: example test
	./example > /dev/null
	./test

bench: benchmark.cpp $(OBJS)
	g++ -std=gnu++17 -Wall -O3 -o $@ $^ -I../modp ../modp/modp_b64.o -lbenchmark ../proxygen/proxygen/lib/.libs/libproxygenlib.a  -lssl -lcrypto -lglog

runbench: bench
	./bench

clean:
	rm -f *.o test example fb64 $(STATIC_LIB)

fb64: fb64.c $(STATIC_LIB)
	gcc -Wall -O3 -o $@ $^
