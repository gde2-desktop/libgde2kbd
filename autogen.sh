#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="libgde2kbd"

(test -f $srcdir/configure.ac) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}

which gde2-autogen || {
    echo "You need to install gde2-common from the GDE2 Git"
    exit 1
}

REQUIRED_AUTOMAKE_VERSION=1.9
USE_GDE22_MACROS=1
USE_COMMON_DOC_BUILD=yes

. gde2-autogen

