#include "MyItem.h"
#include "QPainter"


MyItem::MyItem(QgsMapCanvas *map)
	:QgsMapCanvasItem(map)
{

}


MyItem::~MyItem(void)
{
}

void MyItem::paint(QPainter * painter)
{
	QPoint pt = (toCanvasCoordinates(m_pt) - pos()).toPoint();
	painter->drawEllipse(pt, 12, 12);
}
