#ifndef MAP_WGT_GLOBAL_H
#define MAP_WGT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MAP_WGT_LIB
# define MAP_WGT_EXPORT Q_DECL_EXPORT
#else
# define MAP_WGT_EXPORT Q_DECL_IMPORT
#endif

#endif // MAP_WGT_GLOBAL_H
