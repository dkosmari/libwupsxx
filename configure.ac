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

AC_CANONICAL_HOST

DEVKITPRO_WUT_INIT

AM_INIT_AUTOMAKE([foreign subdir-objects])

LT_INIT([disable-shared])

AC_PROG_CXX
AC_LANG([C++])
AX_APPEND_COMPILE_FLAGS([-std=c++23], [CXX])


DEVKITPRO_WUT_SETUP
WIIU_ENV_SETUP_WUPS([external/WiiUPluginSystem])
WIIU_ENV_SETUP_LIBBUTTONCOMBO([external/libbuttoncombo])
WIIU_ENV_SETUP_LIBNOTIFICATIONS([external/libnotifications])


WANT_NOTIFY=no
WANT_SHORTCUTS=no


AC_ARG_ENABLE([demo],
              [AS_HELP_STRING([--enable-demo],
                              [Build the libwupsxx-demo plugin.])],
              [
                  AS_VAR_IF([enableval], [yes],
                            [
                                WANT_NOTIFY=yes
                                WANT_SHORTCUTS=yes
                            ])
              ])
AM_CONDITIONAL([BUILD_DEMO], [test x$enable_demo = xyes])


AC_ARG_ENABLE([notify],
              [AS_HELP_STRING([--enable-notify],
                              [Enable suport for Notifications module.])],
              [
                  WANT_NOTIFY=$enableval
              ])
AM_CONDITIONAL([USE_NOTIFY], [test x$WANT_NOTIFY = xyes])


AC_ARG_ENABLE([shortcuts],
              [AS_HELP_STRING([--enable-shortcuts],
                              [Enable support for ButtonCombo module.])],
              [
                  WANT_SHORTCUTS=$enableval
              ])
AM_CONDITIONAL([USE_SHORTCUTS], [test x$WANT_SHORTCUTS = xyes])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
