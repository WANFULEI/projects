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
#include "QgsMapLayerRegistry.h"
#include "tinyxml.h"
#include "log.h"
#include <QFile>
#undef min
#undef max
#include "QgsRasterLayer.h"
#include "qgsvectorlayer.h"

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
	m_map = 0;
}

Map3D::~Map3D()
{

}

Q_EXPORT_PLUGIN2(Map3D, Map3D)

void Map3D::loadMap(){
	m_map = new Map;
	QString mapConfigFile = qApp->applicationDirPath() + "/../config/mapConfig.xml";
	TiXmlDocument doc(mapConfigFile.toStdString().c_str());
	if(!doc.LoadFile()){
		LOG_ERROR << tr("Component=Map3D，加载地图配置文件%1失败！").arg(mapConfigFile).toStdString();
		return;
	}
	TiXmlElement *rootNode = doc.RootElement();
	if(rootNode == 0) return;
	TiXmlElement *xmlNode = rootNode->FirstChildElement();
	while(xmlNode){
		if(QString(xmlNode->Value()) == "Layer"){
			loadLayer(xmlNode);
		}else if(QString(xmlNode->Value()) == "Group"){
			loadGroup(xmlNode);
		}
		xmlNode = xmlNode->NextSiblingElement();
	}

	GlobalInstance::getInstance()->setMap3D(m_map);
}


void Map3D::initialize(){
	osgViewer::Viewer *viewer = new osgViewer::Viewer;
	osg::Group* root = new osg::Group();
	viewer->setSceneData( root );
	loadMap();
	MapNode *mapNode = new MapNode(m_map);
    root->addChild( mapNode );
	m_manip = new EarthManipulator();
	viewer->setCameraManipulator( m_manip );

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
	connect(m_mapCanvas, SIGNAL(layersChanged()), this, SLOT(slotLayersChanged()));
	connect(QgsMapLayerRegistry::instance(), SIGNAL(layersRemoved(const QStringList&)), this, SLOT(slotLayersRemoved(const QStringList&)));
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

void Map3D::slotLayersChanged(){
	if(m_mapCanvas == 0 || m_map == 0) return;
 	QStringList layers = getTypeLayers(m_mapCanvas->mapSettings().layers(), "raster");
	for(int i=0; i<m_map->getNumImageLayers(); ++i){
		ImageLayer *layer = m_map->getImageLayerAt(i);
		bool find = false;
		for(int j=0; j<layers.size(); ++j){
			if(layers[j].left(layer->getName().length()) == layer->getName().c_str()){
				find = true;
				break;
			}
		}
		if(find){
			if(!layer->getVisible()) layer->setVisible(true);
		}else{
			QStringList layers = getTypeLayers(mapLayers2Layers(QgsMapLayerRegistry::instance()->mapLayers()), "raster");
			for(int j=0; j<layers.size(); ++j){
				if(layers[j].left(layer->getName().length()) == layer->getName().c_str()){
					find = true;
					break;
				}
			}
			if(find && layer->getVisible()) layer->setVisible(false);
		}
	}

	for(int i=0; i<m_map->getNumElevationLayers(); ++i){
		ElevationLayer *layer = m_map->getElevationLayerAt(i);
		bool find = false;
		for(int j=0; j<layers.size(); ++j){
			if(layers[j].left(layer->getName().length()) == layer->getName().c_str()){
				find = true;
				break;
			}
		}
		if(find){
			if(!layer->getVisible()) layer->setVisible(true);
		}else{
			QStringList layers = getTypeLayers(mapLayers2Layers(QgsMapLayerRegistry::instance()->mapLayers()), "raster");
			for(int j=0; j<layers.size(); ++j){
				if(layers[j].left(layer->getName().length()) == layer->getName().c_str()){
					find = true;
					break;
				}
			}
			if(find && layer->getVisible()) layer->setVisible(false);
		}
	}

	int index = 0;
	for(int i=0; i<layers.size(); ++i){
		for(int j=0; j<m_map->getNumImageLayers(); ++j){
			ImageLayer *layer = m_map->getImageLayerAt(j);
			if(layer == 0) continue;
			if(layers[i].left(layer->getName().length()) == layer->getName().c_str()){
				m_map->moveImageLayer(layer, m_map->getNumImageLayers() - (index++) - 1);
				break;
			}
		}
	}

	index = 0;
	for(int i=0; i<layers.size(); ++i){
		for(int j=0; j<m_map->getNumElevationLayers(); ++j){
			ElevationLayer *layer = m_map->getElevationLayerAt(j);
			if(layer == 0) continue;
			if(layers[i].left(layer->getName().length()) == layer->getName().c_str()){
				m_map->moveElevationLayer(layer, m_map->getNumImageLayers() - (index++) - 1);
				break;
			}
		}
	}

	layers = getTypeLayers(m_mapCanvas->mapSettings().layers(), "vector");
	for(int i=0; i<m_map->getNumModelLayers(); ++i){
		ModelLayer *layer = m_map->getModelLayerAt(i);
		bool find = false;
		for(int j=0; j<layers.size(); ++j){
			if(layers[j].left(layer->getName().length()) == layer->getName().c_str()){
				find = true;
				break;
			}
		}
		if(find){
			if(!layer->getVisible()) layer->setVisible(true);
		}else{
			QStringList layers = getTypeLayers(mapLayers2Layers(QgsMapLayerRegistry::instance()->mapLayers()), "vector");
			for(int j=0; j<layers.size(); ++j){
				if(layers[j].left(layer->getName().length()) == layer->getName().c_str()){
					find = true;
					break;
				}
			}
			if(find && layer->getVisible()) layer->setVisible(false);
		}
	}

	index = 0;
	for(int i=0; i<layers.size(); ++i){
		for(int j=0; j<m_map->getNumModelLayers(); ++j){
			ModelLayer *layer = m_map->getModelLayerAt(j);
			if(layer == 0) continue;
			if(layers[i].left(layer->getName().length()) == layer->getName().c_str()){
				m_map->moveModelLayer(layer, m_map->getNumImageLayers() - (index++) - 1);
				break;
			}
		}
	}
}

void Map3D::slotLayersRemoved( const QStringList& layers){
	for(int i=0; i<layers.size(); ++i){
		removeLayer(layers[i]);
	}
}

void Map3D::removeLayer(QString name){
	for(int i=0; i<m_map->getNumImageLayers(); ++i){
		ImageLayer *layer = m_map->getImageLayerAt(i);
		if(name.left(layer->getName().length()) == layer->getName().c_str()){
			m_map->removeImageLayer(layer);
			break;
		}
	}
}

void Map3D::loadGroup(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return;
	if(QString(xmlNode->Value()) != "Group") return;
	xmlNode = xmlNode->FirstChildElement();
	while (xmlNode){
		if(QString(xmlNode->Value()) == "Layer"){
			loadLayer(xmlNode);
		}else if(QString(xmlNode->Value()) == "Group"){
			loadGroup(xmlNode);
		}
		xmlNode = xmlNode->NextSiblingElement();
	}
}

void Map3D::loadLayer(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return;
	if(QString(xmlNode->Value()) != "Layer") return;
	QString type = xmlNode->Attribute("Type");
	QString name = xmlNode->Attribute("Name");
	QString path = xmlNode->Attribute("Path");
	type = type.toLower();
	path = qApp->applicationDirPath() + "/../" + path;
	if(!QFile::exists(path)){
		LOG_ERROR << tr("Component=Map3D，地图文件%1不存在！Row=%2").arg(path).arg(xmlNode->Row()).toStdString();
		return;
	}
	if(name.isEmpty()){
		LOG_ERROR << tr("Component=Map3D，图层名称为空，请指定图层名称！Row=%2").arg(path).arg(xmlNode->Row()).toStdString();
		return;
	}
	if(type == "raster"){
		GDALOptions gdal;
		gdal.url() = path.toStdString();
		ImageLayer *layer = new ImageLayer(name.toStdString(), gdal);
		m_map->addImageLayer(layer);
	}else if(type == "vector"){

	}
}

bool Map3D::addRasterLayer(QString layerName, QString rasterFilePath)
{
	if(m_map == 0){
		LOG_ERROR << "m_map=0, maybe Map3D component not load.";
		return false;
	}
	if(layerName.isEmpty()){
		LOG_ERROR << "addRasterLayer with a empty layer name is not allowed.";
		return false;
	}
	if(!QFile::exists(rasterFilePath)){
		LOG_ERROR << tr("raster file=%1 not exist.").arg(rasterFilePath).toStdString();
		return false;
	}
	GDALOptions gdal;
	gdal.url() = rasterFilePath.toStdString();
	ImageLayer *layer = new ImageLayer(layerName.toStdString(), gdal);
	m_map->insertImageLayer(layer, m_map->getNumImageLayers());
	return true;
}

QStringList Map3D::getTypeLayers(QStringList layers, QString type)
{
	if(type.toLower() == "raster"){
		for(int i = layers.size()-1; i >= 0; --i){
			if(dynamic_cast<QgsRasterLayer *>(QgsMapLayerRegistry::instance()->mapLayer(layers[i])) == 0)
				layers.removeAt(i);
		}
		return layers;
	}else if(type.toLower() == "vector"){
		for(int i = layers.size()-1; i >= 0; --i){
			if(dynamic_cast<QgsVectorLayer *>(QgsMapLayerRegistry::instance()->mapLayer(layers[i])) == 0)
				layers.removeAt(i);
		}
		return layers;
	}else{
		return layers;
	}
}

QStringList Map3D::mapLayers2Layers(QMap<QString, QgsMapLayer *> mapLayers)
{
	QStringList layers;
	for(auto iter = mapLayers.begin(); iter != mapLayers.end(); ++iter){
		layers << iter.key();
	}
	return layers;
}
