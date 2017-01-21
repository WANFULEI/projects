#include "Keypoint.h"


Keypoint::Keypoint(void)
{
	setGraphicType(GraphicItem::Point);
}


Keypoint::~Keypoint(void)
{
}

QgsPoint Keypoint::getVertex()
{
	return m_pt;
}
