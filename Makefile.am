#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libtask/libtask.la

libtask_libtask_la_SOURCES = \
	libtask/386-ucontext.h \
	libtask/amd64-ucontext.h \
	libtask/asm.S \
	libtask/channel.c \
	libtask/context.c \
	libtask/fd.c \
	libtask/mips-ucontext.h \
	libtask/net.c \
	libtask/power-ucontext.h \
	libtask/print.c \
	libtask/qlock.c \
	libtask/rendez.c \
	libtask/task.c \
	libtask/taskimpl.h

libtask_libtask_la_CFLAGS = \
	-I$(top_srcdir)/libtask

libtask_libtask_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

include_HEADERS = libtask/task.h

check_PROGRAMS = \
	tests/httpload \
	tests/primes \
	tests/tcpproxy \
	tests/testdelay \
	tests/testdelay1

tests_httpload_SOURCES = tests/httpload.c
tests_httpload_CFLAGS = -I$(top_srcdir)/libtask
tests_httpload_LDADD = $(top_builddir)/libtask/libtask.la

tests_primes_SOURCES = tests/primes.c
tests_primes_CFLAGS = -I$(top_srcdir)/libtask
tests_primes_LDADD = $(top_builddir)/libtask/libtask.la

tests_tcpproxy_SOURCES = tests/tcpproxy.c
tests_tcpproxy_CFLAGS = -I$(top_srcdir)/libtask
tests_tcpproxy_LDADD = $(top_builddir)/libtask/libtask.la

tests_testdelay_SOURCES = tests/testdelay.c
tests_testdelay_CFLAGS = -I$(top_srcdir)/libtask
tests_testdelay_LDADD = $(top_builddir)/libtask/libtask.la

tests_testdelay1_SOURCES = tests/testdelay1.c
tests_testdelay1_CFLAGS = -I$(top_srcdir)/libtask
tests_testdelay1_LDADD = $(top_builddir)/libtask/libtask.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libtask.pc
