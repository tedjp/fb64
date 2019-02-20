# This file is part of fb64.
#
# Copyright (c) 2019 Ted J. Percival
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

.PHONY: all check clean coverage runbench install uninstall

STATIC_LIB = libfb64.a

CC = gcc
CFLAGS = -std=gnu11 -pipe -fPIC -Wall -g -O3
COMPILE_OBJ = $(CC) $(CFLAGS) -shared -fvisibility=hidden -c
COMPILE = $(CC) $(CFLAGS)

OBJS = encode.o decode.o

all: fb64 $(STATIC_LIB)

%.o: %.c
	$(COMPILE_OBJ) $(COVERAGE_FLAGS) $<

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
	$(COMPILE) $(COVERAGE_FLAGS) -o $@ $^

example: example.c $(OBJS)
	$(COMPILE) $(COVERAGE_FLAGS) -o $@ $^

check: example test
	./example > /dev/null
	./test

bench: benchmark.cpp $(OBJS)
	g++ -std=gnu++17 -Wall -O3 -o $@ $^ -I../modp ../modp/modp_b64.o -lbenchmark ../proxygen/proxygen/lib/.libs/libproxygenlib.a  -lssl -lcrypto -lglog

runbench: bench
	./bench

clean:
	rm -f *.o test example fb64 $(STATIC_LIB)

coverage:
	$(MAKE) clean
	$(MAKE) COVERAGE_FLAGS="--coverage -O0"
	$(MAKE) check COVERAGE_FLAGS="--coverage -O0"

fb64: fb64.c $(STATIC_LIB)
	$(COMPILE) $(COVERAGE_FLAGS) -o $@ $^
