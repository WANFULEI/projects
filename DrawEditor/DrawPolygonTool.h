#pragma once
#include "drawtool.h"
#include "osg\Vec3d"

class QgsRubberBand;
class DrawPolygonTool :
	public DrawTool
{
public:
	DrawPolygonTool(QgsMapCanvas *map);
	~DrawPolygonTool(void);

protected:
	virtual void canvasMoveEvent(QgsMapMouseEvent* e);

	virtual void canvasPressEvent(QgsMapMouseEvent* e);

	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);

private:
	QgsRubberBand *m_rubberBand;
	QVector<osg::Vec3d> m_points;
};


