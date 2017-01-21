#include "GraphicItem.h"
#include "QUuid"


GraphicItem::GraphicItem(void)
{
	m_graphicType = None;
	m_borderColor = Qt::red;
	m_borderWidth = 2;
	m_borderStyle = Qt::SolidLine;
	m_isOutline = true;
	m_outLineColor = Qt::black;
	m_outLineWidth = 1;
	m_fillColor = QColor(255, 255, 0, 128);
	m_ID = QUuid::createUuid().toString();
	m_iconWidth = 24;
	m_iconHeight = 24;
	m_editor = 0;
}


GraphicItem::~GraphicItem(void)
{
}
