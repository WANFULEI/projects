#include "tt4.h"
#include <QtGui/QResizeEvent>

tt4::tt4(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_pMyWidget = dynamic_cast<Qt_default_widget *>(centralWidget());
}

tt4::~tt4()
{

}

void tt4::on_actionPan_triggered()
{
	m_pMyWidget->select_tool(Pan);
}

void tt4::on_actionSelect_triggered()
{
	m_pMyWidget->select_tool(Select);
}
