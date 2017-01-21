#pragma once
#include "graphicitem.h"

class Polyline :
	public GraphicItem
{
public:
	Polyline(void);
	~Polyline(void);

	virtual QList<QgsPoint> getVertexs() { return m_points; }
	virtual void setVertexs(QList<QgsPoint> pts) { m_points = pts; }

private:
	QList<QgsPoint> m_points;
};

