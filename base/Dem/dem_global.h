#ifndef DEM_GLOBAL_H
#define DEM_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DEM_LIB
# define DEM_EXPORT Q_DECL_EXPORT
#else
# define DEM_EXPORT Q_DECL_IMPORT
#endif

#endif // DEM_GLOBAL_H
