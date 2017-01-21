#pragma once
#include "qgsmapmouseevent.h"

class InputEvent
{
public:
	InputEvent(void);
	~InputEvent(void);

	virtual void canvasMoveEvent(QgsMapMouseEvent* e) {}

	virtual void canvasPressEvent(QgsMapMouseEvent* e) {}

	virtual void canvasReleaseEvent(QgsMapMouseEvent* e) {}

	virtual void keyPressEvent(QKeyEvent* e) {}

	virtual void keyReleaseEvent(QKeyEvent* e) {}

};

