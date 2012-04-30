#                                        -*- Automake -*-
# Process this file with automake to produce Makefile.in.

lib_LTLIBRARIES = libtask.la

libtask_la_SOURCES = \
	386-ucontext.h \
	amd64-ucontext.h \
	asm.S \
	channel.c \
	context.c \
	fd.c \
	mips-ucontext.h \
	net.c \
	power-ucontext.h \
	print.c \
	qlock.c \
	rendez.c \
	task.c \
	taskimpl.h

libtask_la_CFLAGS = \
	-I$(srcdir)

libtask_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

pkgincludedir = $(includedir)
pkginclude_HEADERS = task.h
