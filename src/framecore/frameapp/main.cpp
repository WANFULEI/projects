#include "frameapp.h"
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QFile>

int main(int argc, char *argv[])
{
	QTextCodec * codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForTr(codec);

	QApplication a(argc, argv);

// 	QFile style("C:/Program Files (x86)/qgis2.15.0/resources/themes/Night Mapping/style.qss");
// 	style.open(QIODevice::ReadOnly);
// 	a.setStyleSheet(style.readAll());

	frameapp w;
	w.show();
	return a.exec();
}
