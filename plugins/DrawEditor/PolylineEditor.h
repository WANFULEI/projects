#pragma once
#include "graphiceditor.h"
#include "QgsHighlight.h"
#include "QgsVertexMarker.h"
#include "qgsrubberband.h"

class PolylineEditor :
	public GraphicEditor
{
public:
	PolylineEditor(void);
	~PolylineEditor(void);

	virtual void canvasMoveEvent(QgsMapMouseEvent* e);

	virtual void canvasPressEvent(QgsMapMouseEvent* e);

	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);

private:
	void highlight();

	virtual void onDeActive();

private:
	//QgsHighlight *m_highlight;
	QList<QgsVertexMarker *> m_markers;
	QgsRubberBand *m_rubberBand;
	QPoint m_ptDown;
};

