#ifndef MAP3D_GLOBAL_H
#define MAP3D_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MAP3D_LIB
# define MAP3D_EXPORT Q_DECL_EXPORT
#else
# define MAP3D_EXPORT Q_DECL_IMPORT
#endif

#endif // MAP3D_GLOBAL_H
