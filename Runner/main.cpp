#include "runner.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle("ribbonstyle");
	Runner w;
	w.show();
	return a.exec();
}
