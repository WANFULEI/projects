#ifndef LAYERWIDGET_GLOBAL_H
#define LAYERWIDGET_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MAPWIDGET_LIB
# define LAYERWIDGET_EXPORT Q_DECL_EXPORT
#else
# define LAYERWIDGET_EXPORT Q_DECL_IMPORT
#endif

#endif // LAYERWIDGET_GLOBAL_H
