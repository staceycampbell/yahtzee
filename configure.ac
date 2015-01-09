AC_INIT(yahtzee, 2.1)
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])
# AC_CONFIG_HEADERS([config.h])     # not even really needed
AC_PROG_CC                        # or AC_PROG_CXX
PKG_CHECK_MODULES([ncurses], [ncurses])
AC_CHECK_LIB([m], [sqrt])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
