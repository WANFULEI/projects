#pragma once

#include "qmath.h"
#include <QObject>
#include "component_global.h"
#include "QMainWindow"
#define CORE_EXPORT __declspec(dllimport)
#define GUI_EXPORT __declspec(dllimport)
#define APP_EXPORT __declspec(dllimport)
#define noexcept
#include "qgsmapcanvas.h"
#include "QgsLayerTreeGroup.h"
#include "osg\Group"
#include "osgEarth\Map"
#include "osgEarth\MapNode"
#include "osgEarthUtil\EarthManipulator"
#include "osgViewer\Viewer"


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

	osg::Group *getRoot() const { return m_root; }
	void setRoot(osg::Group *root) { m_root = root; }

	osgEarth::MapNode *getMapNode() const { return m_mapNode; }
	void setMapNode(osgEarth::MapNode *mapNode) { m_mapNode = mapNode; }

	osgEarth::Util::EarthManipulator *getManipulator() const { return m_manipulator; }
	void setManipulator(osgEarth::Util::EarthManipulator *manipulator) { m_manipulator = manipulator; }

	osgViewer::Viewer *getViewer() const { return m_viewer; }
	void setViewer(osgViewer::Viewer *viewer) { m_viewer = viewer; }

private:
	QMainWindow *m_mainWindow;
	QgsMapCanvas *m_map2D;
	QgsLayerTreeGroup *m_layerTreeRoot;
	osgEarth::Map *m_map3D;
	osg::Group *m_root;
	osgEarth::MapNode *m_mapNode;
	osgEarth::Util::EarthManipulator *m_manipulator;
	osgViewer::Viewer *m_viewer;
};

#define global GlobalInstance::getInstance()
