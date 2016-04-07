#include "tt4.h"
#include "mywidget.h"
#include <QtGui/QResizeEvent>

tt4::tt4(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_pMyWidget = dynamic_cast<Qt_widget2 *>(centralWidget());
}

tt4::~tt4()
{

}

void tt4::on_actionPan_triggered()
{
	m_pMyWidget->select_tool(Pan);
}

void tt4::on_actionZoomIn_triggered()
{
	m_pMyWidget->select_tool(Select);
}
