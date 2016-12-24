

INCLUDEPATH += $$quote($$QTITANDIR/include)

SOURCES += \
    $$QTITANDIR/src/ribbon/QtnRibbonSliderPane.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonStatusBar.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonTabBar.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonSystemPopupBar.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonPage.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonGroup.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonBar.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonPrivate.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonMainWindow.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonButton.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonToolTip.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonQuickAccessBar.cpp \
    $$QTITANDIR/src/ribbon/QtnOfficePopupColorButton.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonGallery.cpp \
    $$QTITANDIR/src/ribbon/QtnOfficePopupMenu.cpp \
    $$QTITANDIR/src/ribbon/QtnRibbonBackstageView.cpp \

HEADERS += \
    $$QTITANDIR/src/ribbon/QtnOfficeDefines.h \
    $$QTITANDIR/src/ribbon/QtnRibbonSliderPane.h \
    $$QTITANDIR/src/ribbon/QtnRibbonStatusBar.h \
    $$QTITANDIR/src/ribbon/QtnRibbonTabBar.h \
    $$QTITANDIR/src/ribbon/QtnRibbonSystemPopupBar.h \
    $$QTITANDIR/src/ribbon/QtnRibbonPage.h \
    $$QTITANDIR/src/ribbon/QtnRibbonGroup.h \
    $$QTITANDIR/src/ribbon/QtnRibbonBar.h \
    $$QTITANDIR/src/ribbon/QtnRibbonBarPrivate.h \
	$$QTITANDIR/src/ribbon/QtnRibbonGroupPrivate.h \
    $$QTITANDIR/src/ribbon/QtnRibbonPrivate.h \
    $$QTITANDIR/src/ribbon/QtnRibbonMainWindow.h \
    $$QTITANDIR/src/ribbon/QtnRibbonButton.h \
    $$QTITANDIR/src/ribbon/QtnRibbonToolTip.h \
    $$QTITANDIR/src/ribbon/QtnRibbonQuickAccessBar.h \
    $$QTITANDIR/src/ribbon/QtnOfficePopupColorButton.h \
    $$QTITANDIR/src/ribbon/QtnRibbonGallery.h \
    $$QTITANDIR/src/ribbon/QtnRibbonGalleryPrivate.h \
    $$QTITANDIR/src/ribbon/QtnOfficePopupMenu.h \
    $$QTITANDIR/src/ribbon/QtnRibbonBackstageView.h \
    $$QTITANDIR/src/ribbon/QtnRibbonBackstageViewPrivate.h \

win32 {

    SOURCES += \
        $$QTITANDIR/src/ribbon/QtnOfficeFrameHelper_win.cpp
    
    HEADERS += \
        $$QTITANDIR/src/ribbon/QtnOfficeFrameHelper_win.h
}

unix {
    SOURCES += 
}

RESOURCES += \
    $$QTITANDIR/src/ribbon/QtnRibbonResources.qrc


win32 {
    DEFINES += _WINDOWS
    LIBS += -luser32 -lgdi32 -lshell32
}
