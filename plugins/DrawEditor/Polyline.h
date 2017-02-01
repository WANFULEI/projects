#pragma once
#include "graphicitem.h"

class PolylineEditor;
class Polyline :
	public GraphicItem
{
public:
	Polyline(void);
	~Polyline(void);

	virtual bool isSelect(QgsMapMouseEvent *event);

private:
	static PolylineEditor *m_defaultEditor;
};

