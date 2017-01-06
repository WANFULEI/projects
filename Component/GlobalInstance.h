#pragma once
#include <QObject>
#include "component_global.h"

class QgsMapCanvas;
namespace osgEarth{
	class MapNode;
}
class COMPONENT_EXPORT GlobalInstance
{
public:
	GlobalInstance(void);
	~GlobalInstance(void);

	static GlobalInstance *getInstance();
	QObject *getMainWindow() const { return m_mainWindow; }
	void setMainWindow(QObject *mainWindow) { m_mainWindow = mainWindow; }

	QgsMapCanvas *getMap2D() const { return m_map2D; }
	void setMap2D(QgsMapCanvas *map2D) { m_map2D = map2D; }

	osgEarth::MapNode *getMap3D() const { return m_map3D; }
	void setMap3D(osgEarth::MapNode *map3D) { m_map3D = map3D; }

private:
	QObject *m_mainWindow;
	QgsMapCanvas *m_map2D;
	osgEarth::MapNode *m_map3D;
};

