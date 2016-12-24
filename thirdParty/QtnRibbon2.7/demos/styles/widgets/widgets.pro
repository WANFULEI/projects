TEMPLATE = app

QTITANDIR = $$quote($$(QTITANDIR))

include($$QTITANDIR/src/shared/qtitanribbon.pri)

DESTDIR = $$QTITANDIR/lib
DLLDESTDIR = $$QTITANDIR/bin

DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS       = widgets.h \
                dockwidgets.h
				
SOURCES       = main.cpp \
                widgets.cpp \
                dockwidgets.cpp
                
RESOURCES     = widgets.qrc
FORMS         = widgetsdlg.ui

QT += webkit

include($$QTITANDIR/demos/shared/aboutdialog.pri)

macx {
    CONFIG-=app_bundle
}
