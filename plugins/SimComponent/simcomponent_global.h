#ifndef SIMCOMPONENT_GLOBAL_H
#define SIMCOMPONENT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SIMCOMPONENT_LIB
# define SIMCOMPONENT_EXPORT Q_DECL_EXPORT
#else
# define SIMCOMPONENT_EXPORT Q_DECL_IMPORT
#endif

#endif // SIMCOMPONENT_GLOBAL_H
