#ifndef FRAMEGUI_GLOBAL_H
#define FRAMEGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef FRAMEGUI_LIB
# define FRAMEGUI_EXPORT Q_DECL_EXPORT
#else
# define FRAMEGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // FRAMEGUI_GLOBAL_H
