if BUILD_TESTS
TEST = test
endif

SUBDIRS = libgde2kbd $(TEST) po

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection --with-gtk=$(GTK_API_VERSION)

DISTCLEANFILES = \
  intltool-extract \
  intltool-merge \
  intltool-update

EXTRA_DIST = \
        autogen.sh \
        libgde2kbd.pc.in \
        libgde2kbdui.pc.in \
        intltool-extract.in \
        intltool-merge.in \
        intltool-update.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgde2kbd.pc libgde2kbdui.pc

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

