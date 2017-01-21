#include "stdafx.h"
#include "map3d.h"
#include "GlobalInstance.h"
#include <osgEarth/ElevationQuery>
#include "QGraphicsLineItem"

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

	double height = 0;
	mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), 110, 50, &height);

	LogarithmicDepthBuffer *buf = new LogarithmicDepthBuffer;
	buf->install( viewer->getCamera() );

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
	connect(QgsMapLayerRegistry::instance(), SIGNAL(layersWillBeRemoved(const QStringList&)), this, SLOT(slotLayersRemoved(const QStringList&)));
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
				//m_map->moveModelLayer(layer, m_map->getNumImageLayers() - (index++) - 1);
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
	for(int i=0; i<m_map->getNumImageLayers(); ++i){
		ImageLayer *layer = m_map->getImageLayerAt(i);
		if(name.left(layer->getName().length()) == layer->getName().c_str()){
			QgsMapLayer *mapLayer = QgsMapLayerRegistry::instance()->mapLayer(name);
			if(dynamic_cast<QgsRasterLayer *>(mapLayer)){
				m_map->removeImageLayer(layer);
			}
			break;
		}
	}
	for(int i=0; i<m_map->getNumElevationLayers(); ++i){
		ElevationLayer *layer = m_map->getElevationLayerAt(i);
		if(name.left(layer->getName().length()) == layer->getName().c_str()){
			QgsMapLayer *mapLayer = QgsMapLayerRegistry::instance()->mapLayer(name);
			if(dynamic_cast<QgsRasterLayer *>(mapLayer)){
				m_map->removeElevationLayer(layer);
			}
			break;
		}
	}
	for(int i=0; i<m_map->getNumModelLayers(); ++i){
		ModelLayer *layer = m_map->getModelLayerAt(i);
		if(name.left(layer->getName().length()) == layer->getName().c_str()){
			QgsMapLayer *mapLayer = QgsMapLayerRegistry::instance()->mapLayer(name);
			if(dynamic_cast<QgsVectorLayer *>(mapLayer)){
				m_map->removeModelLayer(layer);
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

bool Map3D::addVectorLayer(QString layerName, QString filePath)
{
	if(m_map == 0){
		LOG_ERROR << "m_map=0, maybe Map3D component not load.";
		return false;
	}
	if(!QFile::exists(filePath)){
		LOG_ERROR << tr("vector file=%1 not exist.").arg(filePath).toStdString();
		return false;
	}
	if(layerName.isEmpty()){
		LOG_ERROR << "addVectorLayer with a empty layer name is not allowed.";
		return false;
	}

	Style style;
	GDALDataset *poDS = (GDALDataset *)GDALOpenEx( filePath.toStdString().c_str(), GDAL_OF_VECTOR, 0, 0, 0 );
	if(poDS == 0) return false;
	OGRLayer *layer = poDS->GetLayer(0);
	if(layer == 0) return false;
	layer->ResetReading();
	OGRFeature *feature = layer->GetNextFeature();
	if(feature == 0) return false;
	if(wkbFlatten(feature->GetGeometryRef()->getGeometryType()) == wkbPoint){
// 		PointSymbol *pointSymbol = style.getOrCreateSymbol<PointSymbol>();
// 		pointSymbol->fill()->color() = Color::Red;
// 		pointSymbol->size() = 12;

		osg::Image *image = osgDB::readImageFile("c:\\osgeo4w\\images\\flags\\vi.png");
		IconSymbol *markerSymbol = style.getOrCreateSymbol<IconSymbol>();
		//markerSymbol->setImage(image);
		markerSymbol->placement() = IconSymbol::PLACEMENT_VERTEX;
		markerSymbol->url() = StringExpression("c:\\osgeo4w\\images\\flags\\vi.png");
//		osg::Node *node = osgDB::readNodeFile("D:\\osg-file-path\\cow.osgt");
// 		markerSymbol->setModel(node);
// 		markerSymbol->alignment() = MarkerSymbol::ALIGN_CENTER_CENTER;
// 		markerSymbol->scale() = 1000;
	}else{
		LineSymbol *lineSymbol = style.getOrCreateSymbol<LineSymbol>();
		lineSymbol->stroke()->color() = Color::Yellow;
		lineSymbol->stroke()->width() = 1.0f;
	}
	OGRFeature::DestroyFeature(feature);
	GDALClose( poDS );

	TextSymbol* text = style.getOrCreateSymbol<TextSymbol>();
	text->font() = "黑体";
	text->encoding()= osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	text->content() = StringExpression( "[name]" );
	//text->priority() = NumericExpression( "[pop_cntry]" );
	text->removeDuplicateLabels() = true;
	text->size() = 12;
	text->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
	text->fill()->color() = Color::White;
	text->halo()->color() = Color::Black;
	
	OGRFeatureOptions featureOptions;
	featureOptions.url() = filePath.toStdString();

	FeatureGeomModelOptions geomOptions;
	geomOptions.featureOptions() = featureOptions;
	geomOptions.styles() = new StyleSheet();
	geomOptions.styles()->addStyle(style);


	m_map->addModelLayer(new ModelLayer(layerName.toStdString(), geomOptions));
	return true;
}

bool Map3D::createLayer(QString layerName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool Map3D::removeLayer(QString layerName)
{
	throw std::logic_error("The method or operation is not implemented.");
}
