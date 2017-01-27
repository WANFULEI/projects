#pragma once
#include "graphicitem.h"
class Polygon :
	public GraphicItem
{
public:
	Polygon(void);
	~Polygon(void);

	virtual bool isSelect(QgsMapMouseEvent *event);

private:
};

