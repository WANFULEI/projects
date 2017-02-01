#include "map2d.h"
#include "Component\GlobalInstance.h"
#include "component\Log.h"
#include "QgsLayerTreeMapCanvasBridge.h"
#include "QgsProviderRegistry.h"
#include "QBoxLayout"
#include "QAction"
#include "QgsMapToolZoom.h"
#include "QgsMapToolPan.h"
#include "QgsMapLayerRegistry.h"
#include "QgsRasterLayer.h"
#include "QgsLayerTreeModel.h"
#include "MenuProvider.h"
#include "QToolBar"
#include "QDockWidget"
#include "qgsvectorlayer.h"
#include "QgsLayerTreeViewDefaultActions.h"
#include "QgsLayerTreeLayer.h"


Map2D::Map2D()
{
	m_layerTree = 0;
}

Map2D::~Map2D()
{
	//delete global->getMap2D();
	delete m_tools.m_zoomIn;
	delete m_tools.m_zoomOut;
	delete m_tools.m_pan;
	delete m_tools.m_select;
	delete m_tools.m_measureDist;
}

Q_EXPORT_PLUGIN2(Map2D, Map2D)

void Map2D::loadMap(){
	global->setMap2D(new QgsMapCanvas);
	global->setLayerTreeRoot(new QgsLayerTreeGroup);

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
			loadLayer(xmlNode, global->getLayerTreeRoot());
		}else if(QString(xmlNode->Value()) == "Group"){
			QString name = xmlNode->Attribute("Name");
			if(name.isEmpty()){
				LOG_ERROR << tr("Component=Map2D, group name is empty!, Row=%1").arg(xmlNode->Row()).toStdString();
				xmlNode = xmlNode->NextSiblingElement();
				continue;
			}
			loadGroup(xmlNode, global->getLayerTreeRoot()->addGroup(name));
		}
		xmlNode = xmlNode->NextSiblingElement();
	}

	new QgsLayerTreeMapCanvasBridge(global->getLayerTreeRoot(), global->getMap2D());
	global->setLayerTreeRoot(global->getLayerTreeRoot());
	global->getMap2D()->setCachingEnabled(true);
	global->getMap2D()->setParallelRenderingEnabled(true);
	global->getMap2D()->setPreviewMode(QgsPreviewEffect::PreviewProtanope);
}

void Map2D::initialize(){
	//设置驱动目录
	QgsProviderRegistry::instance(qApp->applicationDirPath() + "/../apps/qgis/plugins");
	
	loadMap();
	initTools();
	
	QWidget *map2DWidget = GlobalInstance::getInstance()->getMainWindow()->findChild<QWidget *>("Map2DWidget");
	if(map2DWidget) {
		QHBoxLayout *hBox = new QHBoxLayout;
		hBox->addWidget(global->getMap2D());
		map2DWidget->setLayout(hBox);
	}else {
		global->getMap2D()->show();
	}

	createLayerView();

	//设置action状态
	QAction *action = dynamic_cast<QAction *>(GlobalInstance::getInstance()->getMainWindow()->findChild<QObject *>("ZoomToNext"));
	if(action){
		action->setEnabled(false);
		connect(global->getMap2D(), SIGNAL(zoomNextStatusChanged(bool)), action, SLOT(setEnabled(bool)));
	}
	action = dynamic_cast<QAction *>(GlobalInstance::getInstance()->getMainWindow()->findChild<QObject *>("ZoomToLast"));
	if(action){
		action->setEnabled(false);
		connect(global->getMap2D(), SIGNAL(zoomLastStatusChanged(bool)), action, SLOT(setEnabled(bool)));
	}

	//设置漫游状态
	slotReset();
	slotPan();
}

void Map2D::initTools()
{
	m_tools.m_zoomIn = new QgsMapToolZoom( global->getMap2D(), false /* zoomIn */ );
	m_tools.m_zoomOut = new QgsMapToolZoom( global->getMap2D(), true /* zoomOut */ );
	m_tools.m_pan = new QgsMapToolPan( global->getMap2D() );
	//m_tools.m_measureDist = new QgsMeasureTool(global->getMap2D(), false);
}

void Map2D::slotPan()
{
	if(global->getMap2D() == 0) return;
	global->getMap2D()->setMapTool(m_tools.m_pan);
}

void Map2D::slotZoomIn()
{
	if(global->getMap2D() == 0) return;
	global->getMap2D()->setMapTool(m_tools.m_zoomIn);
}

void Map2D::slotZoomOut()
{
	if(global->getMap2D() == 0) return;
	global->getMap2D()->setMapTool(m_tools.m_zoomOut);
}

void Map2D::slotReset()
{
	if(global->getMap2D() == 0) return;
	global->getMap2D()->setExtent(global->getMap2D()->fullExtent());
	global->getMap2D()->refresh();
}

void Map2D::slotZoomToLayer(){
	if(global->getMap2D() == 0) return;
	//QgsMapLayer *l;
	//m_mapCanvas->setExtent(l->extent());
}

void Map2D::slotZoomToLast(){
	if(global->getMap2D() == 0) return;
	global->getMap2D()->zoomToPreviousExtent();
}

void Map2D::slotZoomToNext(){
	if(global->getMap2D() == 0) return;
	global->getMap2D()->zoomToNextExtent();
}

void Map2D::slotRefresh(){
	if(global->getMap2D() == 0) return;
	global->getMap2D()->refresh();
}

void Map2D::slotRemoveLayer(){
	if(m_layerTree == 0) return;
	QList<QgsLayerTreeNode *> nodes = m_layerTree->selectedNodes(true);
	for(int i=0; i<nodes.size(); ++i){
		QgsMapLayerRegistry::instance()->removeMapLayers(getLayers(nodes[i]));
	}
	for(int i=0; i<nodes.size(); ++i){
		QgsLayerTreeGroup *group = dynamic_cast<QgsLayerTreeGroup *>(nodes[i]->parent());
		if(group) {
			group->removeChildNode(nodes[i]);
		}
	}
}

void Map2D::loadGroup(TiXmlElement *xmlNode, QgsLayerTreeGroup *group)
{
	if(xmlNode == 0 || group == 0) return;
	if(QString(xmlNode->Value()) != "Group") return;
	xmlNode = xmlNode->FirstChildElement();
	while (xmlNode){
		if(QString(xmlNode->Value()) == "Layer"){
			loadLayer(xmlNode, group);
		}else if(QString(xmlNode->Value()) == "Group"){
			QString name = xmlNode->Attribute("Name");
			if(name.isEmpty()){
				LOG_ERROR << tr("Component=Map2D, group name is empty!, Row=%1").arg(xmlNode->Row()).toStdString();
				xmlNode = xmlNode->NextSiblingElement();
				continue;
			}
			loadGroup(xmlNode, group->addGroup(name));
		}
		xmlNode = xmlNode->NextSiblingElement();
	}
}

void Map2D::loadLayer(TiXmlElement *xmlNode, QgsLayerTreeGroup *group)
{
	if(xmlNode == 0 || group == 0) return;
	if(QString(xmlNode->Value()) != "Layer") return;
	QString type = xmlNode->Attribute("Type");
	QString name = xmlNode->Attribute("Name");
	QString path = xmlNode->Attribute("Path");
	type = type.toLower();
	if(!QFile::exists(path)){
		path = qApp->applicationDirPath() + "/../" + path;
	}
	if(!QFile::exists(path)){
		LOG_ERROR << tr("Component=Map2D，地图文件%1不存在！Row=%2").arg(path).arg(xmlNode->Row()).toStdString();
		return;
	}
	if(name.isEmpty()){
		LOG_ERROR << tr("Component=Map2D，图层名称为空，请指定图层名称！Row=%2").arg(path).arg(xmlNode->Row()).toStdString();
		return;
	}
	if(type == "raster"){
		QgsRasterLayer *layer = new QgsRasterLayer(path, name);
		QgsMapLayerRegistry::instance()->addMapLayer(layer);
		group->addLayer(layer);
	}else if(type == "vector"){

	}
}

void Map2D::createLayerView()
{
	//建立图层管理器窗口
	QgsLayerTreeModel *model = new QgsLayerTreeModel(global->getLayerTreeRoot() , this );
	model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
	model->setFlag( QgsLayerTreeModel::AllowNodeRename );
	model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
	model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
	//model->setFlag( QgsLayerTreeModel::ShowRasterPreviewIcon );
	model->setAutoCollapseLegendNodes( 10 );
	m_layerTree = new QgsLayerTreeView;
	m_layerTree->setModel( model );
	m_layerTree->setMenuProvider(new MenuProvider);

	//添加组菜单项
	QAction* actionAddGroup = new QAction( tr( "添加组" ), this );
	actionAddGroup->setIcon( QIcon(qApp->applicationDirPath() + "/../resources/images/themes/default/mActionAddGroup.svg" ) );
	actionAddGroup->setToolTip( tr( "添加组" ) );
	connect( actionAddGroup, SIGNAL( triggered( bool ) ), m_layerTree->defaultActions(), SLOT( addGroup() ) );
	
	//扩展收起菜单项
	QAction* actionExpandAll = new QAction( tr( "展开" ), this );
	actionExpandAll->setIcon( QIcon(qApp->applicationDirPath() + "/../resources/images/themes/default/mActionExpandTree.svg" ) );
	actionExpandAll->setToolTip( tr( "展开" ) );
	connect( actionExpandAll, SIGNAL( triggered( bool ) ), m_layerTree, SLOT( expandAllNodes() ) );
	QAction* actionCollapseAll = new QAction( tr( "收起" ), this );
	actionCollapseAll->setIcon( QIcon(qApp->applicationDirPath() + "/../resources/images/themes/default/mActionCollapseTree.svg" ) );
	actionCollapseAll->setToolTip( tr( "收起" ) );
	connect( actionCollapseAll, SIGNAL( triggered( bool ) ), m_layerTree, SLOT( collapseAllNodes() ) );

	//移除菜单项
	QAction *actionRemoveLayer = new QAction(0);
	actionRemoveLayer->setIcon( QIcon(qApp->applicationDirPath() + "/../resources/images/themes/default/mActionRemoveLayer.svg" ) );
	actionRemoveLayer->setToolTip(tr("删除"));
	connect(actionRemoveLayer, SIGNAL(triggered()), this, SLOT(slotRemoveLayer()));

	QToolBar* toolbar = new QToolBar();
	toolbar->setIconSize( QSize( 16, 16 ) );
	toolbar->addAction( actionAddGroup );
	toolbar->addAction( actionExpandAll );
	toolbar->addAction( actionCollapseAll );
	toolbar->addAction( actionRemoveLayer );

	QVBoxLayout* vboxLayout = new QVBoxLayout;
	vboxLayout->setMargin( 0 );
	vboxLayout->addWidget( toolbar );
	vboxLayout->addWidget( m_layerTree );

	QWidget* w = new QWidget;
	w->setLayout( vboxLayout );
	QDockWidget *pDock = GlobalInstance::getInstance()->getMainWindow()->findChild<QDockWidget *>("LayerView");
	if(pDock){
		pDock->setWidget(w);
	}else{
		w->show();
	}
}

QList<QgsMapLayer *> Map2D::getLayers(QgsLayerTreeNode *node)
{
	QList<QgsMapLayer *> layers;
	if(node == 0) return layers;
	if(node->nodeType() == QgsLayerTreeNode::NodeLayer){
		layers << dynamic_cast<QgsLayerTreeLayer *>(node)->layer();
		return layers;
	}else if(node->nodeType() == QgsLayerTreeNode::NodeGroup){
		auto children = node->children();
		for(int i=0; i<children.size(); ++i){
			layers += getLayers(children[i]);
		}
	}
	return layers;
}

void Map2D::slotMeasureDist()
{
	if(global->getMap2D() == 0) return;
	global->getMap2D()->setMapTool(m_tools.m_measureDist);
}

bool Map2D::addRasterLayer(QString layerName, QString rasterFilePath)
{
	if(global->getLayerTreeRoot() == 0){
		LOG_ERROR << "m_rootGrou=0, maybe Map2D component not load.";
		return false;
	}
	if(!QgsRasterLayer::isValidRasterFileName(rasterFilePath)){
		LOG_ERROR << "invalid raster file=" << rasterFilePath.toStdString() << ".";
		return false;
	}
	if(layerName.isEmpty()){
		LOG_ERROR << "addRasterLayer with a empty layer name is not allowed.";
		return false;
	}
	QgsRasterLayer *layer = new QgsRasterLayer(rasterFilePath, layerName);
	QgsMapLayerRegistry::instance()->addMapLayer(layer);
	global->getLayerTreeRoot()->insertLayer(0, layer);
	return true;
}

bool Map2D::addVectorLayer(QString layerName, QString filePath)
{
	if(global->getLayerTreeRoot() == 0){
		LOG_ERROR << "m_rootGrou=0, maybe Map2D component not load.";
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
	QgsVectorLayer *layer = new QgsVectorLayer(filePath, layerName, "ogr", false);
	if(!layer->isValid()){
		QString msg = tr( "%1 is not a valid or recognized data source" ).arg( filePath );
		LOG_ERROR << msg.toStdString();
		delete layer;
		return false;
	}
	QgsMapLayerRegistry::instance()->addMapLayer(layer);
	global->getLayerTreeRoot()->insertLayer(0, layer);
	return true;
}

bool Map2D::createLayer(QString layerName)
{
	return false;
}

bool Map2D::removeLayer(QString layerName)
{
	return false;
}

bool Map2D::addElevationLayer(QString layerName, QString filePath)
{
	return addRasterLayer(layerName, filePath);
}

void Map2D::slotRightRotate()
{
// 	global->getMap2D()->freeze(true);
 	global->getMap2D()->setRotation(global->getMap2D()->rotation() + 15);
// 	global->getMap2D()->freeze(false);
// 	global->getMap2D()->refresh();
}

void Map2D::slotLeftRotate()
{
// 	global->getMap2D()->freeze(true);
 	global->getMap2D()->setRotation(global->getMap2D()->rotation() - 15);
// 	global->getMap2D()->freeze(false);
// 	global->getMap2D()->refresh();
}

void Map2D::slotNorth()
{
// 	global->getMap2D()->freeze(true);
 	global->getMap2D()->setRotation(0);
// 	global->getMap2D()->freeze(false);
// 	global->getMap2D()->refresh();
}
