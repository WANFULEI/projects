#include "map3d.h"
#include "component/GlobalInstance.h"
#include "osgEarth/Map"
#include "Component/log.h"
#include "osgEarth/MapNode"
#include "QBoxLayout"
#include "osgEarthUtil/VerticalScale"
#include "QgsMapLayerRegistry.h"
#include "osgEarth/TerrainEngineNode"
#include "osgEarth/Viewpoint"
#include "osgEarthUtil/Sky"
#include "Map3DOptions.h"
#include "QgsRasterLayer.h"
#include "qgsvectorlayer.h"
#include "osgEarthQt/ViewerWidget"
#include "osgEarthDrivers/gdal/GDALOptions"

Map3D::Map3D()
{
	m_map3DOptionsDlg = 0;
}

Map3D::~Map3D()
{

}

Q_EXPORT_PLUGIN2(Map3D, Map3D)

void Map3D::loadMap(){
	global->setMap3D(new osgEarth::Map);

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
}


void Map3D::initialize(){
	osgViewer::Viewer *viewer = new osgViewer::Viewer;
	global->setViewer(viewer);
	osg::Group* root = new osg::Group();
	viewer->setSceneData( root );
	global->setRoot(root);
	loadMap();
	osgEarth::MapNode *mapNode = new osgEarth::MapNode(global->getMap3D());
    root->addChild( mapNode );
	global->setMapNode(mapNode);

	global->setManipulator(new osgEarth::Util::EarthManipulator);
	viewer->setCameraManipulator(global->getManipulator());

// 	LogarithmicDepthBuffer *buf = new LogarithmicDepthBuffer;
// 	buf->install( viewer->getCamera() );

	auto viewerWidget = new osgEarth::QtGui::ViewerWidget(viewer);
	QWidget *w = global->getMainWindow()->findChild<QWidget *>("Map3DWidget");
	if(w) {
		QHBoxLayout *hBox = new QHBoxLayout;
		hBox->addWidget(viewerWidget);
		w->setLayout(hBox);
	}else {
		w->show();
	}

	auto verticalScale = new osgEarth::Util::VerticalScale;
	verticalScale->setScale(1);
	global->getMapNode()->getTerrainEngine()->addEffect(verticalScale);

	connect(global->getMap2D(), SIGNAL(extentsChanged()), this, SLOT(slot_extentsChanged()));
	connect(global->getMap2D(), SIGNAL(layersChanged()), this, SLOT(slotLayersChanged()));
	connect(QgsMapLayerRegistry::instance(), SIGNAL(layersWillBeRemoved(const QStringList&)), this, SLOT(slotLayersRemoved(const QStringList&)));

	addSky();
}

void Map3D::slot_extentsChanged(){
	QgsPoint centerPt = global->getMap2D()->center();
	QgsRectangle extent = global->getMap2D()->extent();
	global->getManipulator()->setViewpoint( osgEarth::Viewpoint(
        "Viewpoint",
        centerPt.x(), centerPt.y(), 0,   // longitude, latitude, altitude
		0, -21.6, extent.width() * 80000), // heading, pitch, range
         3 );                    // duration
}

void Map3D::slotLayersChanged(){
	if(global->getMap3D() == 0) return;
 	QStringList layers = getTypeLayers(global->getMap2D()->mapSettings().layers(), "raster");
	for(int i=0; i<global->getMap3D()->getNumImageLayers(); ++i){
		osgEarth::ImageLayer *layer = global->getMap3D()->getImageLayerAt(i);
		bool find = false;
		for(int j=0; j<layers.size(); ++j){
			if(layers[j] == convertEarthLayerName(layer->getName().c_str())){
				find = true;
				break;
			}
		}
		if(find){
			if(!layer->getVisible()) layer->setVisible(true);
		}else{
			QStringList layers = getTypeLayers(mapLayers2Layers(QgsMapLayerRegistry::instance()->mapLayers()), "raster");
			for(int j=0; j<layers.size(); ++j){
				if(layers[j] == convertEarthLayerName(layer->getName().c_str())){
					find = true;
					break;
				}
			}
			if(find && layer->getVisible()) layer->setVisible(false);
		}
	}

	for(int i=0; i<global->getMap3D()->getNumElevationLayers(); ++i){
		osgEarth::ElevationLayer *layer = global->getMap3D()->getElevationLayerAt(i);
		bool find = false;
		for(int j=0; j<layers.size(); ++j){
			if(layers[j] == convertEarthLayerName(layer->getName().c_str())){
				find = true;
				break;
			}
		}
		if(find){
			if(!layer->getVisible()) layer->setVisible(true);
		}else{
			QStringList layers = getTypeLayers(mapLayers2Layers(QgsMapLayerRegistry::instance()->mapLayers()), "raster");
			for(int j=0; j<layers.size(); ++j){
				if(layers[j] == convertEarthLayerName(layer->getName().c_str())){
					find = true;
					break;
				}
			}
			if(find && layer->getVisible()) layer->setVisible(false);
		}
	}

	int index = 0;
	for(int i=0; i<layers.size(); ++i){
		for(int j=0; j<global->getMap3D()->getNumImageLayers(); ++j){
			osgEarth::ImageLayer *layer = global->getMap3D()->getImageLayerAt(j);
			if(layer == 0) continue;
			if(layers[i] == convertEarthLayerName(layer->getName().c_str())){
				int pos = global->getMap3D()->getNumImageLayers() - (index++) - 1;
				if(pos >=0 && pos < global->getMap3D()->getNumImageLayers())
					global->getMap3D()->moveImageLayer(layer, pos);
				break;
			}
		}
	}

	index = 0;
	for(int i=0; i<layers.size(); ++i){
		for(int j=0; j<global->getMap3D()->getNumElevationLayers(); ++j){
			osgEarth::ElevationLayer *layer = global->getMap3D()->getElevationLayerAt(j);
			if(layer == 0) continue;
			if(layers[i] == convertEarthLayerName(layer->getName().c_str())){
				int pos = global->getMap3D()->getNumElevationLayers() - (index++) - 1;
				if(pos >=0 && pos < global->getMap3D()->getNumElevationLayers())
					global->getMap3D()->moveElevationLayer(layer, pos);
				break;
			}
		}
	}

	layers = getTypeLayers(global->getMap2D()->mapSettings().layers(), "vector");
	for(int i=0; i<global->getMap3D()->getNumModelLayers(); ++i){
		osgEarth::ModelLayer *layer = global->getMap3D()->getModelLayerAt(i);
		bool find = false;
		for(int j=0; j<layers.size(); ++j){
			if(layers[j] == convertEarthLayerName(layer->getName().c_str())){
				find = true;
				break;
			}
		}
		if(find){
			if(!layer->getVisible()) layer->setVisible(true);
		}else{
			QStringList layers = getTypeLayers(mapLayers2Layers(QgsMapLayerRegistry::instance()->mapLayers()), "vector");
			for(int j=0; j<layers.size(); ++j){
				if(layers[j] == convertEarthLayerName(layer->getName().c_str())){
					find = true;
					break;
				}
			}
			if(find && layer->getVisible()) layer->setVisible(false);
		}
	}

	index = 0;
	for(int i=0; i<layers.size(); ++i){
		for(int j=0; j<global->getMap3D()->getNumModelLayers(); ++j){
			osgEarth::ModelLayer *layer = global->getMap3D()->getModelLayerAt(j);
			if(layer == 0) continue;
			if(layers[i] == convertEarthLayerName(layer->getName().c_str())){
				int pos = global->getMap3D()->getNumModelLayers() - (index++) - 1;
				if(pos >=0 && pos < global->getMap3D()->getNumModelLayers())
					global->getMap3D()->moveModelLayer(layer, pos);
				break;
			}
		}
	}
}

void Map3D::slotLayersRemoved( const QStringList& layers){
	for(int i=0; i<layers.size(); ++i){
		myRemoveLayer(layers[i]);
	}
}

void Map3D::myRemoveLayer(QString name){
	for(int i=0; i<global->getMap3D()->getNumImageLayers(); ++i){
		osgEarth::ImageLayer *layer = global->getMap3D()->getImageLayerAt(i);
		if(name.left(layer->getName().length()) == layer->getName().c_str()){
			QgsMapLayer *mapLayer = QgsMapLayerRegistry::instance()->mapLayer(name);
			if(dynamic_cast<QgsRasterLayer *>(mapLayer)){
				global->getMap3D()->removeImageLayer(layer);
			}
			break;
		}
	}
	for(int i=0; i<global->getMap3D()->getNumElevationLayers(); ++i){
		osgEarth::ElevationLayer *layer = global->getMap3D()->getElevationLayerAt(i);
		if(name.left(layer->getName().length()) == layer->getName().c_str()){
			QgsMapLayer *mapLayer = QgsMapLayerRegistry::instance()->mapLayer(name);
			if(dynamic_cast<QgsRasterLayer *>(mapLayer)){
				global->getMap3D()->removeElevationLayer(layer);
			}
			break;
		}
	}
	for(int i=0; i<global->getMap3D()->getNumModelLayers(); ++i){
		osgEarth::ModelLayer *layer = global->getMap3D()->getModelLayerAt(i);
		if(name.left(layer->getName().length()) == layer->getName().c_str()){
			QgsMapLayer *mapLayer = QgsMapLayerRegistry::instance()->mapLayer(name);
			if(dynamic_cast<QgsVectorLayer *>(mapLayer)){
				global->getMap3D()->removeModelLayer(layer);
			}
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
	if(!QFile::exists(path)){
		path = qApp->applicationDirPath() + "/../" + path;
	}
	if(!QFile::exists(path)){
		LOG_ERROR << tr("Component=Map3D，地图文件%1不存在！Row=%2").arg(path).arg(xmlNode->Row()).toStdString();
		return;
	}
	if(name.isEmpty()){
		LOG_ERROR << tr("Component=Map3D，图层名称为空，请指定图层名称！Row=%2").arg(path).arg(xmlNode->Row()).toStdString();
		return;
	}
	if(type == "raster"){
		osgEarth::Drivers::GDALOptions gdal;
		gdal.url() = path.toStdString();
		osgEarth::ImageLayer *layer = new osgEarth::ImageLayer(name.toStdString(), gdal);
		global->getMap3D()->addImageLayer(layer);
	}else if(type == "vector"){

	}
}


QStringList Map3D::getTypeLayers(QStringList layers, QString type)
{
	if(type.toLower() == "raster"){
		for(int i = layers.size()-1; i >= 0; --i){
			if(dynamic_cast<QgsRasterLayer *>(QgsMapLayerRegistry::instance()->mapLayer(layers[i])) == 0)
				layers.removeAt(i);
		}
		//return layers;
	}else if(type.toLower() == "vector"){
		for(int i = layers.size()-1; i >= 0; --i){
			if(dynamic_cast<QgsVectorLayer *>(QgsMapLayerRegistry::instance()->mapLayer(layers[i])) == 0)
				layers.removeAt(i);
		}
		//return layers;
	}else{
		//return layers;
	}
	for(auto i = layers.begin(); i != layers.end(); ++i){
		*i = (*i).left((*i).length() - 17);
	}
	return layers;
}

QStringList Map3D::mapLayers2Layers(QMap<QString, QgsMapLayer *> mapLayers)
{
	QStringList layers;
	for(auto iter = mapLayers.begin(); iter != mapLayers.end(); ++iter){
		layers << iter.key();
	}
	return layers;
}

QString Map3D::convertEarthLayerName(QString name)
{
	return name.replace('-', '_');
}

void Map3D::slotMap3DOptions()
{
	if(m_map3DOptionsDlg == 0){
		m_map3DOptionsDlg = new Map3DOptions(global->getMainWindow());
	}
	m_map3DOptionsDlg->show();
}

void Map3D::addSky()
{
	auto sky = osgEarth::Extension::create("sky_simple", osgEarth::ConfigOptions());
	global->getMapNode()->addExtension(sky);
	auto parents = global->getMapNode()->getParents();
	for(int i=0; i<parents.size(); ++i){
		osgEarth::Util::SkyNode *sky = dynamic_cast<osgEarth::Util::SkyNode *>(parents[i]);
		if(sky){
			sky->setLighting(0);
			break;
		}
	}
}

