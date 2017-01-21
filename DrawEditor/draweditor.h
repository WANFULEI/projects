#ifndef DRAWEDITOR_H
#define DRAWEDITOR_H

#include "draweditor_global.h"
#include "component.h"

class DrawEditorVps;
class IconDlg;
class DrawKeypointTool;
class DrawPolylineTool;
class DrawEditor : public Component
{
	Q_OBJECT
public:
	DrawEditor();
	~DrawEditor();

	virtual void initialize();

private slots:
	void slotSelect();
	void slotDrawPoint();
	void slotDrawPolyline();

private:
	DrawEditorVps *m_vps;
	DrawKeypointTool *m_drawKeypointTool;
	DrawPolylineTool *m_drawPolylineTool;
};

#endif // DRAWEDITOR_H
