#ifndef EXCEPTION_GLOBAL_H
#define EXCEPTION_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef EXCEPTION_LIB
# define EXCEPTION_EXPORT Q_DECL_EXPORT
#else
# define EXCEPTION_EXPORT Q_DECL_IMPORT
#endif

#endif // EXCEPTION_GLOBAL_H
