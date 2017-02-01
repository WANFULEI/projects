#pragma once
#include "drawtool.h"
#include "QgsRubberBand.h"
#include "osg\Vec3d"

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
	QVector<osg::Vec3d> m_points;
};

