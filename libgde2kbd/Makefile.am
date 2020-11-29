lib_LTLIBRARIES = libgde2kbd.la libgde2kbdui.la

uidir = $(pkgdatadir)/ui
ui_DATA = show-layout.ui

common_CFLAGS = \
	-I$(top_srcdir)					\
	$(GDK_CFLAGS)					\
	$(GIO_CFLAGS)					\
	$(LIBXKLAVIER_CFLAGS)				\
	$(XLIB_CFLAGS)					\
	-I$(top_srcdir)/intl				\
	-DG_LOG_DOMAIN=\"Gde2KbdIndicator\"		\
	-DDATADIR=\"$(datadir)\"			\
	-DSYSCONFDIR=\"$(sysconfdir)\"			\
	-DLIBDIR=\"$(libdir)\"

libgde2kbd_la_CFLAGS = $(common_CFLAGS)

libgde2kbdui_la_CFLAGS = $(common_CFLAGS)		\
	-DUIDIR=\"$(uidir)\"				\
	$(GTK_CFLAGS)

common_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

common_LIBADD = \
		$(GDK_LIBS) \
		$(GIO_LIBS) \
		$(LIBXKLAVIER_LIBS) \
		$(XLIB_LIBS)

libgde2kbd_la_LDFLAGS = $(common_LDFLAGS)
libgde2kbd_la_LIBADD =  $(common_LIBADD)

libgde2kbdui_la_LDFLAGS = $(common_LDFLAGS)
libgde2kbdui_la_LIBADD = libgde2kbd.la \
                           $(common_LIBADD) \
                           $(GLIB_LIBS) \
                           $(GMODULE_LIBS) \
                           $(GTK_LIBS)

libgde2kbd_la_SOURCES = \
                         gde2kbd-desktop-config.c \
                         gde2kbd-keyboard-config.c \
                         gde2kbd-util.c

libgde2kbdui_la_SOURCES = \
                         gde2kbd-indicator-config.c \
                         gde2kbd-indicator.c \
                         gde2kbd-status.c \
                         gde2kbd-indicator-marshal.c \
                         gde2kbd-keyboard-drawing-marshal.c \
                         gde2kbd-keyboard-drawing.c

BUILT_SOURCES = gde2kbd-indicator-marshal.c \
	gde2kbd-indicator-marshal.h         \
        gde2kbd-keyboard-drawing-marshal.c            \
        gde2kbd-keyboard-drawing-marshal.h

CLEANFILES = $(BUILT_SOURCES)

gde2kbdincdir = $(includedir)/libgde2kbd
gde2kbdinc_HEADERS = \
                      gde2kbd-desktop-config.h \
                      gde2kbd-keyboard-config.h \
                      gde2kbd-indicator.h \
                      gde2kbd-status.h \
                      gde2kbd-indicator-config.h \
                      gde2kbd-keyboard-drawing.h \
                      gde2kbd-util.h

@INTLTOOL_DESKTOP_RULE@

noinst_HEADERS = $(extra_nih) \
                 gde2kbd-config-private.h

@INTLTOOL_XML_NOMERGE_RULE@
gsettingsschema_in_files = org.gde2.peripherals-keyboard-xkb.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

EXTRA_DIST = gde2kbd-indicator-marshal.list \
             gde2kbd-keyboard-drawing-marshal.list \
             $(gsettingsschema_in_files) \
             $(ui_DATA)

DISTCLEANFILES = $(gsettings_SCHEMAS)

GLIB_GENMARSHAL = $(shell pkg-config --variable=glib_genmarshal glib-2.0)

gde2kbd-indicator-marshal.h: gde2kbd-indicator-marshal.list
	$(AM_V_GEN)$(GLIB_GENMARSHAL) --prefix=gde2kbd_indicator $(srcdir)/gde2kbd-indicator-marshal.list --header > $@

gde2kbd-indicator-marshal.c: gde2kbd-indicator-marshal.h
	$(AM_V_GEN)$(GLIB_GENMARSHAL) --prefix=gde2kbd_indicator $(srcdir)/gde2kbd-indicator-marshal.list --body > $@

gde2kbd-keyboard-drawing-marshal.h: gde2kbd-keyboard-drawing-marshal.list
	$(AM_V_GEN)( $(GLIB_GENMARSHAL) --prefix=gde2kbd_keyboard_drawing $(srcdir)/gde2kbd-keyboard-drawing-marshal.list \
                            --header > gde2kbd-keyboard-drawing-marshal.tmp \
          && mv gde2kbd-keyboard-drawing-marshal.tmp gde2kbd-keyboard-drawing-marshal.h ) \
        || ( rm -f gde2kbd-keyboard-drawing-marshal.tmp && exit 1 )

gde2kbd-keyboard-drawing-marshal.c: gde2kbd-keyboard-drawing-marshal.h
	$(AM_V_GEN)( $(GLIB_GENMARSHAL) --prefix=gde2kbd_keyboard_drawing $(srcdir)/gde2kbd-keyboard-drawing-marshal.list \
                            --body > gde2kbd-keyboard-drawing-marshal.tmp \
          && mv gde2kbd-keyboard-drawing-marshal.tmp gde2kbd-keyboard-drawing-marshal.c ) \
        || ( rm -f gde2kbd-keyboard-drawing-marshal.tmp && exit 1 )


-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Gde2kbd-1.0.gir
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(gde2kbdinc_HEADERS) \
        $(libgde2kbd_la_SOURCES) \
        $(libgde2kbdui_la_SOURCES)

Gde2kbd-1.0.gir: libgde2kbd.la libgde2kbdui.la
Gde2kbd_1_0_gir_INCLUDES = GObject-2.0 Gtk-$(GTK_API_VERSION) xlib-2.0 Xkl-1.0
Gde2kbd_1_0_gir_CFLAGS = $(libgde2kbdui_la_CFLAGS)
Gde2kbd_1_0_gir_LIBS = libgde2kbd.la libgde2kbdui.la
Gde2kbd_1_0_gir_FILES = $(introspection_sources)

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif
