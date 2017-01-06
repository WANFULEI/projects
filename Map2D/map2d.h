#ifndef MAP2D_H
#define MAP2D_H

#include "map2d_global.h"
#include "component.h"
#define CORE_EXPORT __declspec(dllimport)
#define GUI_EXPORT __declspec(dllimport)
#define APP_EXPORT __declspec(dllimport)

#include "qgsmapcanvas.h"

class Map2D : public Component 
{
	Q_OBJECT
public:
	Map2D();
	~Map2D();

	class Tools
	{
	public:
		QgsMapTool *m_zoomIn;
        QgsMapTool *m_zoomOut;
        QgsMapTool *m_pan;
		QgsMapTool *m_select;
		Tools(){
			m_zoomIn = 0;
			m_zoomOut = 0;
			m_pan = 0;
			m_select = 0;
		}
		~Tools(){
			
		}
	}m_tools;

protected:
	void initialize();

private:
	void initTools();

private slots:
	void slotPan();
	void slotZoomIn();
	void slotZoomOut();
	void slotReset();

private:
	QgsMapCanvas *m_mapCanvas;
};

#endif // MAP2D_H
