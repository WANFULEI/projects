#ifndef MAP2D_GLOBAL_H
#define MAP2D_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MAP2D_LIB
# define MAP2D_EXPORT Q_DECL_EXPORT
#else
# define MAP2D_EXPORT Q_DECL_IMPORT
#endif

#endif // MAP2D_GLOBAL_H
