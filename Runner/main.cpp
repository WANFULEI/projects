#include "runner.h"
#include <QtGui/QApplication>
#include <qtextcodec.h>

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

	QApplication a(argc, argv);
	a.setStyle("ribbonstyle");
	Runner w;
	w.showMaximized();
	return a.exec();
}
