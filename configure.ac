dnl ***************************************************************************
dnl *** configure.in for libgde2kbd                                        ***
dnl ***************************************************************************
m4_define([libgde2kbd_major_version], [1])
m4_define([libgde2kbd_minor_version], [16])
m4_define([libgde2kbd_micro_version], [0])
m4_define([libgde2kbd_version],[libgde2kbd_major_version.libgde2kbd_minor_version.libgde2kbd_micro_version])

AC_INIT([libgde2kbd], [libgde2kbd_version], [https://github.com/gde2-desktop/libgde2kbd])
AC_PREREQ(2.59)

VERSION_INFO=6:0:2
AC_SUBST(VERSION_INFO)

AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.9 no-dist-gzip dist-xz check-news])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR(m4)

dnl ----------------------------------------------------------------------------
dnl GTK library version
dnl ----------------------------------------------------------------------------

GTK_API_VERSION=2.0
GTK_REQUIRED=2.24
GDK_REQUIRED=2.24

AC_SUBST([GTK_API_VERSION])

dnl ***************************************************************************
dnl *** Minimum library versions for libgde2kbd                            ***
dnl ***************************************************************************
GLIB_REQUIRED=2.36
GIO_REQUIRED=2.25.0

LIBXKLAVIER_REQUIRED=5.2

AC_SUBST([GIO_REQUIRED])
AC_SUBST([GTK_API_VERSION])
AC_SUBST([GTK_REQUIRED])
AC_SUBST([GDK_REQUIRED])
AC_SUBST([LIBXKLAVIER_REQUIRED])

dnl ***************************************************************************

GDE2_COMMON_INIT
GDE2_COMPILE_WARNINGS

AC_PROG_CC
AC_STDC_HEADERS
AM_PROG_LIBTOOL

# ****
# i18n
# ****

GETTEXT_PACKAGE=libgde2kbd
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])

AM_GLIB_GNU_GETTEXT

IT_PROG_INTLTOOL([0.50.1])

# *****************************************************************************

AC_CHECK_FUNCS(setlocale)

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED)
PKG_CHECK_MODULES(GMODULE, gmodule-2.0, [GMODULE_ADD="gmodule-2.0"],[GMODULE_ADD=""])

AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GMODULE_LIBS)

PKG_CHECK_MODULES(GIO, gio-2.0 >= $GIO_REQUIRED)

AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

PKG_CHECK_MODULES(GDK, gdk-$GTK_API_VERSION >= $GDK_REQUIRED)

AC_SUBST(GDK_CFLAGS)
AC_SUBST(GDK_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-$GTK_API_VERSION >= $GTK_REQUIRED gdk-x11-$GTK_API_VERSION >= $GDK_REQUIRED)

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(LIBXKLAVIER, libxklavier >= $LIBXKLAVIER_REQUIRED)

AC_SUBST(LIBXKLAVIER_CFLAGS)
AC_SUBST(LIBXKLAVIER_LIBS)

AC_PATH_XTRA
XLIB_CFLAGS="$X_CFLAGS"
XLIB_LIBS="$X_LIBS -lX11 $X_EXTRA_LIBS"
AC_SUBST(XLIB_CFLAGS)
AC_SUBST(XLIB_LIBS)

GLIB_GSETTINGS

AC_ARG_ENABLE([tests],
              AC_HELP_STRING([--disable-tests], [Dont build the test apps]),,
              [enable_tests=yes])

AM_CONDITIONAL(BUILD_TESTS, test "$enable_tests" = "yes")

AC_SEARCH_LIBS([sqrt], [m])

GOBJECT_INTROSPECTION_CHECK([0.6.7])

AC_OUTPUT([
Makefile
libgde2kbd.pc
libgde2kbdui.pc
po/Makefile.in
libgde2kbd/Makefile
libgde2kbd/org.gde2.peripherals-keyboard-xkb.gschema.xml
test/Makefile
])
