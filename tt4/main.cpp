#include "tt4.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	tt4 w;
	w.show();

	return a.exec();
}
