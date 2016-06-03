#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([convolution], [0.1], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_SEARCH_LIBS([cvCreateImage], [opencv_core])
AC_SEARCH_LIBS([cvThreshold], [opencv_imgproc])
AC_SEARCH_LIBS([sqrt], [m])

# Checks for header files.
PKG_CHECK_MODULES([GTK], [gtk+-3.0 glib-2.0])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
