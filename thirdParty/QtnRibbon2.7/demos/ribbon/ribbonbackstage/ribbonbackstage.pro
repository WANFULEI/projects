TEMPLATE = app

QTITANDIR = $$quote($$(QTITANDIR))

include($$QTITANDIR/src/shared/qtitanribbon.pri)

DESTDIR = $$QTITANDIR/lib
DLLDESTDIR = $$QTITANDIR/bin

DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX

DESTDIR = "$$DESTDIR"
DLLDESTDIR = "$$DLLDESTDIR"

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS        = mainwindow.h \
				 backstagepages.h

SOURCES        = main.cpp \
                 mainwindow.cpp \
				 backstagepages.cpp

FORMS         = infopage.ui \
				savepage.ui \
				helppage.ui \
				saveemailpage.ui \
				savewebpage.ui \

RESOURCES     = ribbonbackstage.qrc

include($$QTITANDIR/demos/shared/aboutdialog.pri)

macx {
    CONFIG-=app_bundle
}
