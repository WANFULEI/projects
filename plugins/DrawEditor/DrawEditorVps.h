#pragma once

#include "qmath.h"
#include <qgsmaptool.h>

class QgsMapCanvas;
class DrawTool;
class GraphicEditor;
class InputEvent;
class DrawEditorVps : public QgsMapTool
{
public:
	DrawEditorVps(QgsMapCanvas *map);
	~DrawEditorVps(void);

	void selectTool(DrawTool *tool);
	DrawTool *getTool() const { return m_drawTool; }

protected:
	virtual void canvasMoveEvent(QgsMapMouseEvent* e);

	virtual void canvasPressEvent(QgsMapMouseEvent* e);

	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);

	virtual void keyPressEvent(QKeyEvent* e);

	virtual void keyReleaseEvent(QKeyEvent* e);


private:
	DrawTool *m_drawTool;
	InputEvent *m_transTo;
};

