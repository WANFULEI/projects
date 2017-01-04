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

using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;
using namespace osgEarth::Util;
using namespace osgEarth::QtGui;


Map3D::Map3D()
{

}

Map3D::~Map3D()
{

}

Q_EXPORT_PLUGIN2(Map3D, Map3D)

void Map3D::initialize(){
	Map* map = new Map();
	GDALOptions gdal;
    gdal.url() = "C:\\OSGeo4W\\data\\NE2_HR_LC_SR_W_DR.tif";
    ImageLayer* layer = new ImageLayer( "world", gdal );
    map->addImageLayer( layer );
	MapNode *mapNode = new MapNode(map);

	QWidget *w = new ViewerWidget(mapNode);
	w->show();
}