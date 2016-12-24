TEMPLATE = lib

TARGET = qtnribbon

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

QTITANDIR = $$quote($$(QTITANDIR))

include($$QTITANDIR/src/shared/platform.pri)

DESTDIR = $$QTITANDIR/lib
DLLDESTDIR = $$QTITANDIR/bin

DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX

win32 {
RC_FILE = ribbon.rc
}

VERSION = 2.7

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanRibbon user interface component for Qt
QMAKE_TARGET_DESCRIPTION = QtitanRibbon user interface component for Qt
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2012 Developer Machines

include($$QTITANDIR/src/ribbon/ribbon.pri)
include($$QTITANDIR/src/styles/styles.pri)

contains(CONFIG, demo) {
include($$QTITANDIR/src/shared/qtitandemo.pri)
}

DEFINES += QTITAN_LIBRARY


