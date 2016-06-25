#include "test_template.h"
#include <QtGui/QApplication>

#include "..\compose_node.h"
#include "..\manager.h"
#include "..\share_obj.h"
#include "..\singleton.h"
#include "..\time_elapsed.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	test_template w;
	w.show();
	return a.exec();
}
