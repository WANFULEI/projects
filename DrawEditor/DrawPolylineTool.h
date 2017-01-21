#pragma once
#include "drawtool.h"
#include "QgsRubberBand.h"

class DrawPolylineTool :
	public DrawTool
{
public:
	DrawPolylineTool(QgsMapCanvas *map);
	~DrawPolylineTool(void);

protected:
	virtual void canvasMoveEvent(QgsMapMouseEvent* e);

	virtual void canvasPressEvent(QgsMapMouseEvent* e);

	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);

private:
	QgsRubberBand *m_rubberBand;
	QList<QgsPoint> m_points;
};

