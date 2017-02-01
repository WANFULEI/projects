#pragma once
#include "graphicitem.h"

class KeypointEditor;
class Keypoint :
	public GraphicItem
{
public:
	Keypoint(void);
	~Keypoint(void);

	virtual bool isSelect(QgsMapMouseEvent *event);

private:
	static KeypointEditor *m_defaultEditor;
};

