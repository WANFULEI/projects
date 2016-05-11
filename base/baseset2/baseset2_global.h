#ifndef BASESET2_GLOBAL_H
#define BASESET2_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef BASESET2_LIB
# define BASESET2_EXPORT Q_DECL_EXPORT
#else
# define BASESET2_EXPORT Q_DECL_IMPORT
#endif

#endif // BASESET2_GLOBAL_H
