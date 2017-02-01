#include "runner.h"
#include <QtGui/QApplication>
#include <qtextcodec.h>
#include "QTranslator"

#define CORE_EXPORT __declspec(dllimport)
#define GUI_EXPORT __declspec(dllimport)
#define APP_EXPORT __declspec(dllimport)
#include "qgsapplication.h"
#include "windows.h"

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

	

	QgsApplication a(argc, argv, true, "");
	a.setOrganizationName("weiqing.leiqian");
	a.setApplicationName("runner");

	QTranslator translatorQGis;
	translatorQGis.load(qApp->applicationDirPath() + "/../i18n/qgis_zh-Hans.qm");
	a.installTranslator(&translatorQGis);
	QTranslator translatorQt;
	translatorQt.load(qApp->applicationDirPath() + "/../translations/qt_zh_CN.qm");
	a.installTranslator(&translatorQt);

	a.setStyle("ribbonstyle");
	Runner w;

	auto i = LoadLibraryA("C:\\OSGeo4W\\apps\\qgis\\bin\\qgis_gui.dll");
	w.showMaximized();

	return a.exec();
}
