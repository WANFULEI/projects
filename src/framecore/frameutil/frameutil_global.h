#ifndef FRAMEUTIL_GLOBAL_H
#define FRAMEUTIL_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef FRAMEUTIL_LIB
# define FRAMEUTIL_EXPORT Q_DECL_EXPORT
#else
# define FRAMEUTIL_EXPORT Q_DECL_IMPORT
#endif

#endif // FRAMEUTIL_GLOBAL_H
