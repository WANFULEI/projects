#include "frameutil.h"
#include <QApplication>
#include <QFile>


FRAMEUTIL_EXPORT QString frameutil::get_full_path(const QString & path)
{
	if (path.indexOf(":") != -1)
	{
		return path;
	}
	else if (QFile::exists(QApplication::applicationDirPath() + "/" + path))
	{
		return QApplication::applicationDirPath() + "/" + path;
	}
	else
	{
		return path;
	}
}

FRAMEUTIL_EXPORT bool frameutil::get_bool_from_string(const QString & s)
{
	if (s == "1" || s.toLower() == "true")
	{
		return true;
	}
	else
	{
		return false;
	}
}
