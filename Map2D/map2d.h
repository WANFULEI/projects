#ifndef MAP2D_H
#define MAP2D_H

#include "map2d_global.h"
#include "component.h"
#define CORE_EXPORT __declspec(dllimport)
#define GUI_EXPORT __declspec(dllimport)
#define APP_EXPORT __declspec(dllimport)

#include "qgsmapcanvas.h"

class MAP2D_EXPORT Map2D : public Component 
{
public:
	Map2D();
	~Map2D();

protected:
	void initialize();

private:
	QgsMapCanvas *m_mapCanvas;
};

#endif // MAP2D_H
