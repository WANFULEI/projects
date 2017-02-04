#ifndef WSIM_GLOBAL_H
#define WSIM_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef WSIM_LIB
# define WSIM_EXPORT Q_DECL_EXPORT
#else
# define WSIM_EXPORT Q_DECL_IMPORT
#endif

#endif // WSIM_GLOBAL_H
