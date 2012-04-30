#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Initialize autoconf
AC_PREREQ([2.68])

# Library versioning
# Making releases:
#     micro_version += 1
#     interface_age += 1
# If functions have been added or backward-compatibility has been broken:
#     interface_age = 0
m4_define([major_version], [0])
m4_define([minor_version], [1])
m4_define([micro_version], [0])
m4_define([interface_age], [0])

# Initialize autoconf & automake
AC_INIT([Libtask], [major_version.minor_version.micro_version],
    [https://github.com/msteinert/libtask/issues], [libtask])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.9 foreign -Wno-portability])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([libtask/task.h])

# Initialize Libtool
LT_PREREQ([2.2])
LT_INIT

# Checks for programs
AM_PROG_AS
AC_PROG_CC_C99

# Enable compiler flags
AX_TRY_CFLAGS([-Wall], [AX_CFLAGS([-Wall])])
AX_TRY_CFLAGS([-Wextra], [AX_CFLAGS([-Wextra])])
AX_TRY_CFLAGS([-pedantic], [AX_CFLAGS([-pedantic])])

# Libtool versioning
LT_CURRENT=m4_eval(minor_version + micro_version - interface_age)
LT_REVISION=interface_age
LT_AGE=m4_eval(m4_eval(minor_version + micro_version) - interface_age)
AC_SUBST([LT_CURRENT])
AC_SUBST([LT_REVISION])
AC_SUBST([LT_AGE])

# Output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	libtask/Makefile
	libtask.pc
	Makefile
	tests/Makefile
])
AC_OUTPUT

# Summary
AC_MSG_RESULT([
$PACKAGE_NAME build options:
	Host: $host
	Version: $VERSION
])
