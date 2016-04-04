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

void tt4::on_actionPan_triggered()
{
	MyWidget()->Pan();
}

void tt4::on_actionZoomIn_triggered()
{
	MyWidget()->ZoomIn();
}
