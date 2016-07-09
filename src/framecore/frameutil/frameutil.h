#ifndef FRAMEUTIL_H
#define FRAMEUTIL_H

#include "frameutil_global.h"
#include <QString>

namespace frameutil
{
	FRAMEUTIL_EXPORT QString get_full_path(const QString & path);
	FRAMEUTIL_EXPORT bool get_bool_from_string(const QString & s);
}

#endif // FRAMEUTIL_H
