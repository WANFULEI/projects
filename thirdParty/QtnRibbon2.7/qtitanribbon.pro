TEMPLATE = subdirs

include(src/shared/platform.pri)

CONFIG += ordered
CONFIG += debug_and_release
CONFIG += build_qtitan

CONFIG(build_qtitan) {

    SUBDIRS = src/shared/ribbon \
              src/ribbondsgn
}

SUBDIRS += \
          demos/ribbon/ribboncontrols \
          demos/ribbon/ribbonmdi \
          demos/ribbon/ribbonsample \
	  demos/ribbon/ribbongalleries \
	  demos/ribbon/ribbonbackstage \
          demos/styles/guioffice \
          demos/styles/widgets

