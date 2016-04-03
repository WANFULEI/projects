#include "tt4.h"
#include "mywidget.h"
#include <QtGui/QResizeEvent>

tt4::tt4(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	MyWidget(dynamic_cast<mywidget *>(centralWidget()));
}

tt4::~tt4()
{

}
