#ifndef COORDINATE_WIDGET_GLOBAL_H
#define COORDINATE_WIDGET_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef COORDINATE_WIDGET_LIB
# define COORDINATE_WIDGET_EXPORT Q_DECL_EXPORT
#else
# define COORDINATE_WIDGET_EXPORT Q_DECL_IMPORT
#endif

#endif // COORDINATE_WIDGET_GLOBAL_H
