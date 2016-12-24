
INCLUDEPATH += $$quote($$QTITANDIR/include)

SOURCES += \
    $$QTITANDIR/src/styles/QtnCommonStyle.cpp \
    $$QTITANDIR/src/styles/QtnOfficeStyle.cpp \
    $$QTITANDIR/src/styles/QtnRibbonStyle.cpp \
    $$QTITANDIR/src/styles/QtnStyleHelpers.cpp \

HEADERS += \
    $$QTITANDIR/src/styles/QtnCommonStyle.h \
    $$QTITANDIR/src/styles/QtnCommonStylePrivate.h \
    $$QTITANDIR/src/styles/QtnOfficeStyle.h \
    $$QTITANDIR/src/styles/QtnRibbonStyle.h \
    $$QTITANDIR/src/styles/QtnStyleHelpers.h \

win32 {
    SOURCES += $$QTITANDIR/src/styles/QtnCommonStyle_win.cpp
}

linux-g++|linux-g++-64 {
    SOURCES += $$QTITANDIR/src/styles/QtnCommonStyle_linux.cpp
}

macx {
    SOURCES += $$QTITANDIR/src/styles/QtnCommonStyle_mac.mm
}
