#include "test_demo_core.h"
#include <QtGui/QApplication>
#include <QTextCodec>


int main(int argc, char *argv[])
{
	QTextCodec * codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForTr(codec);

	QApplication a(argc, argv);
	test_demo_core w;
	w.show();
	return a.exec();
}
