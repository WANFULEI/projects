#ifndef DEMO_CORE_GLOBAL_H
#define DEMO_CORE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DEMO_CORE_LIB
# define DEMO_CORE_EXPORT Q_DECL_EXPORT
#else
# define DEMO_CORE_EXPORT Q_DECL_IMPORT
#endif

#endif // DEMO_CORE_GLOBAL_H
