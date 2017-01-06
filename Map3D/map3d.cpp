#include "map3d.h"
#include <osg/Notify>
#include <osgGA/GUIEventHandler>
#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>

#include <osgEarth/MapNode>

#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/LogarithmicDepthBuffer>

#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/xyz/XYZOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/gdal/GDALOptions>

#include <osgViewer/CompositeViewer>
#include <osgEarthQt/ViewerWidget>
#include "GlobalInstance.h"
#include <QHBoxLayout>

#define CORE_EXPORT __declspec(dllimport)
#define GUI_EXPORT __declspec(dllimport)
#define APP_EXPORT __declspec(dllimport)

#include "qgsmapcanvas.h"


using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;
using namespace osgEarth::Util;
using namespace osgEarth::QtGui;


Map3D::Map3D()
{
	m_mapCanvas = 0;
	m_manip = 0;
}

Map3D::~Map3D()
{

}

Q_EXPORT_PLUGIN2(Map3D, Map3D)

void Map3D::initialize(){
	osgViewer::Viewer *viewer = new osgViewer::Viewer;
	EarthManipulator* manip = new EarthManipulator();
	m_manip = manip;
    viewer->setCameraManipulator( manip );
	osg::Group* root = new osg::Group();
    viewer->setSceneData( root );

	Map* map = new Map();
	GDALOptions gdal;
    gdal.url() = "C:\\OSGeo4W\\data\\NE2_HR_LC_SR_W_DR.tif";
    ImageLayer* layer = new ImageLayer( "world", gdal );
    map->addImageLayer( layer );
	MapNode *mapNode = new MapNode(map);
	GlobalInstance::getInstance()->setMap3D(mapNode);
    root->addChild( mapNode );
	
	ViewerWidget *viewerWidget = new ViewerWidget(viewer);
	QWidget *w = GlobalInstance::getInstance()->getMainWindow()->findChild<QWidget *>("Map3DWidget");
	if(w) {
		QHBoxLayout *hBox = new QHBoxLayout;
		hBox->addWidget(viewerWidget);
		w->setLayout(hBox);
	}else {
		w->show();
	}

	m_mapCanvas = GlobalInstance::getInstance()->getMap2D();
	connect(m_mapCanvas, SIGNAL(extentsChanged()), this, SLOT(slot_extentsChanged()));
}

void Map3D::slot_extentsChanged(){
	if(m_mapCanvas == 0 || m_manip == 0) return;
	QgsPoint centerPt = m_mapCanvas->center();
	QgsRectangle extent = m_mapCanvas->extent();
	m_manip->setViewpoint( Viewpoint(
        "Viewpoint",
        centerPt.x(), centerPt.y(), 0,   // longitude, latitude, altitude
		0, -21.6, extent.width() * 80000), // heading, pitch, range
         3 );                    // duration
}