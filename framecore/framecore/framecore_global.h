#ifndef FRAMECORE_GLOBAL_H
#define FRAMECORE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef FRAMECORE_LIB
# define FRAMECORE_EXPORT Q_DECL_EXPORT
#else
# define FRAMECORE_EXPORT Q_DECL_IMPORT
#endif

#endif // FRAMECORE_GLOBAL_H
