#ifndef VINCENTY_GLOBAL_H
#define VINCENTY_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef VINCENTY_LIB
# define VINCENTY_EXPORT Q_DECL_EXPORT
#else
# define VINCENTY_EXPORT Q_DECL_IMPORT
#endif

#endif // VINCENTY_GLOBAL_H
