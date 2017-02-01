#pragma once
#include "graphiceditor.h"
class KeypointEditor :
	public GraphicEditor
{
public:
	KeypointEditor(void);
	~KeypointEditor(void);

protected:
	virtual void canvasMoveEvent(QgsMapMouseEvent* e);

	virtual void canvasPressEvent(QgsMapMouseEvent* e);

	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);

};

