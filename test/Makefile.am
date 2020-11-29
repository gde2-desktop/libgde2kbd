noinst_PROGRAMS = gde2kbd-indicator-test \
                  gde2kbd-keyboard-drawing-test \
                  gde2kbd-status-test

common_CFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -Wall \
	$(GTK_CFLAGS) \
	$(LIBXKLAVIER_CFLAGS) \
	-DGDE2LOCALEDIR="\"$(datadir)/locale\""
	
common_LDFLAGS = $(GTK_LIBS) \
	$(LIBXKLAVIER_LIBS) \
	$(top_builddir)/libgde2kbd/libgde2kbd.la	\
	$(top_builddir)/libgde2kbd/libgde2kbdui.la

gde2kbd_indicator_test_CFLAGS=$(common_CFLAGS)

gde2kbd_indicator_test_LDFLAGS=$(common_LDFLAGS)

gde2kbd_keyboard_drawing_test_CFLAGS=$(common_CFLAGS)

gde2kbd_keyboard_drawing_test_LDFLAGS=$(common_LDFLAGS)

gde2kbd_status_test_CFLAGS=$(common_CFLAGS)

gde2kbd_status_test_LDFLAGS=$(common_LDFLAGS)
