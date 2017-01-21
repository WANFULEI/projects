#pragma once
#include <QObject>
#include "component_global.h"
#include "QMainWindow"

class QgsMapCanvas;
namespace osgEarth{
	class Map;
}
class QgsLayerTreeGroup;
class COMPONENT_EXPORT GlobalInstance
{
public:
	GlobalInstance(void);
	~GlobalInstance(void);

	static GlobalInstance *getInstance();
	QMainWindow *getMainWindow() const { return m_mainWindow; }
	void setMainWindow(QMainWindow *mainWindow) { m_mainWindow = mainWindow; }

	QgsMapCanvas *getMap2D() const { return m_map2D; }
	void setMap2D(QgsMapCanvas *map2D) { m_map2D = map2D; }

	osgEarth::Map *getMap3D() const { return m_map3D; }
	void setMap3D(osgEarth::Map *map3D) { m_map3D = map3D; }

	QgsLayerTreeGroup *getLayerTreeRoot() const { return m_layerTreeRoot; }
	void setLayerTreeRoot(QgsLayerTreeGroup *root) { m_layerTreeRoot = root; }

private:
	QMainWindow *m_mainWindow;
	QgsMapCanvas *m_map2D;
	osgEarth::Map *m_map3D;
	QgsLayerTreeGroup *m_layerTreeRoot;
};

#define global GlobalInstance::getInstance()
