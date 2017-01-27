#pragma once
#include "qgsmapcanvasitem.h"

class MyItem : public QgsMapCanvasItem
{
public:
	MyItem(QgsMapCanvas *map);
	~MyItem(void);

	virtual void paint(QPainter * painter);


	QgsPoint m_pt;
};

