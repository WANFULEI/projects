#include "test_demo_core.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	test_demo_core w;
	w.show();
	return a.exec();
}
