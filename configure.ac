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

DEVKITPRO_WUT_INIT

AM_INIT_AUTOMAKE([foreign subdir-objects])

DEVKITPRO_WUT_SETUP

AC_PROG_RANLIB
AC_PROG_CXX
AM_PROG_AR

AC_LANG([C++])
AX_APPEND_COMPILE_FLAGS([-std=c++23], [CXX])

WANT_SHORTCUTS=no
WANT_NOTIFY=no

WIIU_ENV_SETUP_WUPS([external/WiiUPluginSystem])
WIIU_ENV_SETUP_LIBNOTIFICATIONS([external/libnotifications])
WIIU_ENV_SETUP_LIBBUTTONCOMBO([external/libbuttoncombo])

AC_ARG_ENABLE([demo],
              [AS_HELP_STRING([--enable-demo],
                              [Build the libwupsxx-demo plugin.])],
              [
                  AS_IF([test x$enableval = xyes],
                        [
                            WANT_SHORTCUTS=yes
                            WANT_NOTIFY=yes
                        ])
              ])
AM_CONDITIONAL([BUILD_DEMO], [test x$enable_demo = xyes])


AC_ARG_ENABLE([shortcuts],
              [AS_HELP_STRING([--enable-shortcuts],
                              [Enable support for ButtonCombo module.])],
              [
                  WANT_SHORTCUTS=$enableval
              ])
AM_CONDITIONAL([USE_SHORTCUTS], [test x$WANT_SHORTCUTS = xyes])

AC_ARG_ENABLE([notify],
              [AS_HELP_STRING([--enable-notify],
                              [Enable suport for Notifications module.])],
              [
                  WANT_NOTIFY=$enableval
              ])
AM_CONDITIONAL([USE_NOTIFY], [test x$WANT_NOTIFY = xyes])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
