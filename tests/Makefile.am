#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

INCLUDES = -I$(top_srcdir)/libtask
LIBS = $(top_builddir)/libtask/libtask.la

check_PROGRAMS = \
	primes \
	httpload \
	tcpproxy \
	testdelay \
	testdelay1

primes_SOURCES = primes.c
httpload_SOURCES = httpload.c
tcpproxy_SOURCES = tcpproxy.c
testdelay_SOURCES = testdelay.c
testdelay1_SOURCES = testdelay1.c
