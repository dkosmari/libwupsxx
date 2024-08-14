#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libwupsxx],
        [0.0],
        [https://github.com/dkosmari/libwupsxx/issues],
        [],
        [https://github.com/dkosmari/libwupsxx])
AC_CONFIG_SRCDIR([src/item.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

WIIU_WUPS_INIT

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_PROG_RANLIB
AC_PROG_CXX
AM_PROG_AR

AC_LANG([C++])
AX_APPEND_COMPILE_FLAGS([-std=c++23], [CXX])


AC_ARG_ENABLE([demo],
              [AS_HELP_STRING([--enable-demo],
                              [Build the libwupsxx-demo plugin.])])
AM_CONDITIONAL([BUILD_DEMO], [test x$enable_demo = xyes])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
