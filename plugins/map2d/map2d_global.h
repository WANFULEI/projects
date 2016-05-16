#ifndef MAP2D_GLOBAL_H
#define MAP2D_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MAP2D_LIB
# define MAP2D_EXPORT Q_DECL_EXPORT
#else
# define MAP2D_EXPORT Q_DECL_IMPORT
#endif

#define CORE_EXPORT Q_DECL_IMPORT
#define GUI_EXPORT Q_DECL_IMPORT
#define APP_EXPORT Q_DECL_IMPORT
#define PYTHON_EXPORT Q_DECL_IMPORT
#define noexcept

#endif // MAP2D_GLOBAL_H
