#pragma once
#include "graphicitem.h"
class Keypoint :
	public GraphicItem
{
public:
	Keypoint(void);
	~Keypoint(void);

	virtual QgsPoint getVertex();
	void setVertex(QgsPoint pt) { m_pt = pt; }

private:
	QgsPoint m_pt;
};

