#ifndef DRAWEDITOR_H
#define DRAWEDITOR_H

#include "draweditor_global.h"
#include "component/component.h"

class DrawEditorVps;
class IconDlg;
class DrawKeypointTool;
class DrawPolylineTool;
class DrawPolygonTool;
class DrawEditor : public Component
{
	Q_OBJECT
public:
	DrawEditor();
	~DrawEditor();

	virtual void initialize();

	void MyMethod();

private slots:
	void slotSelect();
	void slotDrawPoint();
	void slotDrawPolyline();
	void slotDrawPolygon();

private:
	DrawEditorVps *m_vps;
	DrawKeypointTool *m_drawKeypointTool;
	DrawPolylineTool *m_drawPolylineTool;
	DrawPolygonTool *m_drawPolygonTool;
};

#endif // DRAWEDITOR_H
