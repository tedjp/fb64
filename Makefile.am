#.PHONY: all check clean runbench

ACLOCAL_AMFLAGS = -I m4

LDADD = libfb64.la

lib_LTLIBRARIES = libfb64.la
include_HEADERS = fb64.h

libfb64_la_SOURCES = encode.c decode.c
libfb64_la_LDFLAGS = -version-info 0:0:0

check_PROGRAMS = test example

test_SOURCES = test.c
example_SOURCES = example.c

TESTS = $(check_PROGRAMS)

bench_SOURCES = benchmark.cpp
bench_CPPFLAGS = -I../modp
bench_LDADD = $(LDADD) ../proxygen/proxygen/lib/libproxygenlib.la ../modp/modp_b64.o
bench_LDFLAGS = -lbenchmark

if ENABLE_BENCHMARKS
noinst_PROGRAMS = bench

runbench: bench
	./$<
endif

EXTRA_DIST = README.md LICENSE
