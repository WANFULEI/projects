#ifndef COMPONENT_GLOBAL_H
#define COMPONENT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef COMPONENT_LIB
# define COMPONENT_EXPORT Q_DECL_EXPORT
#else
# define COMPONENT_EXPORT Q_DECL_IMPORT
#endif

#endif // COMPONENT_GLOBAL_H
