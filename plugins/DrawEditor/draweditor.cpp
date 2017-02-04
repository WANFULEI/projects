#include "draweditor.h"
#include <QObject>
#include "component/GlobalInstance.h"
#include "QGraphicsScene"
#include <QGraphicsLineItem>
#include <qgsmapcanvas.h>

#include <QgsVectorLayer.h>
#include <qgsgeometry.h>
#include <QgsSingleSymbolRendererV2.h>
#include <QgsMarkerSymbolLayerV2.h>
#include <qgslayertreegroup.h>
#include <qgsmaplayerregistry.h>
#include <QgsFillSymbolLayerV2.h>
#include "DrawEditorVps.h"
#include "IconDlg.h"
#include "GraphicLayer.h"
#include "DrawKeypointTool.h"
#include "DrawPolylineTool.h"
#include "DrawPolygonTool.h"
#include "osgEarth\ModelLayer"
#include "osg\AnimationPath"
#include "osg\PositionAttitudeTransform"
#include "osgEarth\Map"
#include "osgEarthAnnotation\LocalGeometryNode"
#include "osgEarthUtil\ContourMap"
#include "osg\StateSet"
#include "osg\Math"

using namespace osgEarth;

DrawEditor::DrawEditor()
{
	m_vps = 0;
	m_drawKeypointTool = 0;
	m_drawPolylineTool = 0;
	m_drawPolygonTool = 0;
}

DrawEditor::~DrawEditor()
{
	delete m_vps;
	delete m_drawKeypointTool;
	delete m_drawPolylineTool;
	delete m_drawPolygonTool;
}

void DrawEditor::initialize()
{
// 	QgsVectorLayer *layer = new QgsVectorLayer("Point?crs=epsg:4326", "test", "memory");
// 	if(layer->isValid()){
// 		QgsMarkerSymbolLayerV2 *symbolLayer = new QgsSvgMarkerSymbolLayerV2(qApp->applicationDirPath() + "/../images/svg/accommodation/accommodation_alpinehut.svg");
// 		symbolLayer->setSize(6);
// 		QgsSimpleMarkerSymbolLayerV2 *simpleLayer = new QgsSimpleMarkerSymbolLayerV2;
// 		QgsMarkerSymbolV2 *symbol = new QgsMarkerSymbolV2(QgsSymbolLayerV2List() << simpleLayer << symbolLayer);
// 		QgsSingleSymbolRendererV2 *render = new QgsSingleSymbolRendererV2(symbol);
// 		layer->setRendererV2(render);
// 		layer->startEditing();
// 		QgsFeature feature;
// 		feature.setGeometry(QgsGeometry::fromPoint(QgsPoint(10, 10)));
// 		layer->addFeature(feature);
// 		layer->commitChanges();
// 
// 		QgsMapLayerRegistry::instance()->addMapLayer(layer);
// 		GlobalInstance::getInstance()->getLayerTreeRoot()->insertLayer(0, layer);
// 	}
// 	layer = new QgsVectorLayer("polygon?crs=epsg:4326", "polygonLayer", "memory");
// 	if(layer->isValid()){
// 		QgsSimpleFillSymbolLayerV2 *fillLayer = new QgsSimpleFillSymbolLayerV2;
// 		QgsFillSymbolV2 *symbol = new QgsFillSymbolV2(QgsSymbolLayerV2List() << fillLayer);
// 		QgsSingleSymbolRendererV2 *render = new QgsSingleSymbolRendererV2(symbol);
// 		layer->setRendererV2(render);
// 		layer->startEditing();
// 		QgsFeature feature;
// 		feature.setGeometry(QgsGeometry::fromPolygon(QgsPolygon() << (QgsPolyline() << QgsPoint(0, 0) << QgsPoint(10, 0) << QgsPoint(5, 5))));
// 		layer->addFeature(feature);
// 		layer->commitChanges();
// 
// 		QgsMapLayerRegistry::instance()->addMapLayer(layer);
// 		GlobalInstance::getInstance()->getLayerTreeRoot()->insertLayer(0, layer);
// 	}

// 	long srsid = GlobalInstance::getInstance()->getMap2D()->mapSettings().destinationCrs().srsid();
// 	long srid = GlobalInstance::getInstance()->getMap2D()->mapSettings().destinationCrs().postgisSrid();
// 	QString proj4 = GlobalInstance::getInstance()->getMap2D()->mapSettings().destinationCrs().toProj4();

	//use
	m_vps = new DrawEditorVps(GlobalInstance::getInstance()->getMap2D());

	GraphicLayer *graLayer = new GraphicLayer("graphicLayer");
	QgsMapLayerRegistry::instance()->addMapLayer(graLayer);
	GlobalInstance::getInstance()->getLayerTreeRoot()->insertLayer(0, graLayer);
	m_drawKeypointTool = new DrawKeypointTool("../images", GlobalInstance::getInstance()->getMainWindow());
	m_drawPolylineTool = new DrawPolylineTool(global->getMap2D());
	m_drawPolygonTool = new DrawPolygonTool(global->getMap2D());

	MyMethod();

	//osg::Node* glider = osgDB::readNodeFile("glider.osgt");
}

void DrawEditor::slotSelect()
{
	GlobalInstance::getInstance()->getMap2D()->setMapTool(m_vps);
	m_vps->selectTool(0);
}

void DrawEditor::slotDrawPoint()
{
	GlobalInstance::getInstance()->getMap2D()->setMapTool(m_vps);
	m_vps->selectTool(m_drawKeypointTool);
}

void DrawEditor::slotDrawPolyline()
{
	GlobalInstance::getInstance()->getMap2D()->setMapTool(m_vps);
	m_vps->selectTool(m_drawPolylineTool);
}

void DrawEditor::slotDrawPolygon()
{
	GlobalInstance::getInstance()->getMap2D()->setMapTool(m_vps);
	m_vps->selectTool(m_drawPolygonTool);
}

void DrawEditor::MyMethod()
{

	osg::AnimationPath* animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);
	osg::Vec3d position(0, 0, 0);
	osg::Quat rotation(0, 0, 0, 0);
	global->getMap3D()->getSRS()->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians(10.0), osg::DegreesToRadians(10.0), 10000, position.x(), position.y(), position.z());
	//animationPath->insert(0, osg::AnimationPath::ControlPoint(position,rotation));
	for(int i=1; i<100; ++i){
		osg::Vec3d position2(0, 0, 0);
		global->getMap3D()->getSRS()->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians(10.0 + i * 0.1), osg::DegreesToRadians(10.0 + i * 0.1), 10000, position2.x(), position2.y(), position2.z());

		osg::Vec3d tmp = position2-position;
		animationPath->insert(i, osg::AnimationPath::ControlPoint(position2-position,rotation));
		position = position2;
	}

	global->getMap3D()->getSRS()->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians(10.0), osg::DegreesToRadians(10.0), 10000, position.x(), position.y(), position.z());


	osg::Group* model = new osg::Group;
	osg::Node* glider = osgDB::readNodeFile("glider.osgt");

// 	if (glider)
// 	{
		const osg::BoundingSphere& bs = glider->getBound();

		//float size = radius/bs.radius()*0.3f;
		osg::MatrixTransform* positioned = new osg::MatrixTransform;
		positioned->setDataVariance(osg::Object::STATIC);
		
		
		positioned->setMatrix(
			osg::Matrix::scale(100000,100000,100000));
		//positioned->setScale(osg::Vec3d(100000, 100000, 100000));
		//xform->setAttitude(osg::Quat(osg::DegreesToRadians(45.0), osg::Vec3d(0, 1, 0)));
		//positioned->postMult(osg::Matrix::rotate(osg::Quat(osg::inDegrees(90.0), osg::Vec3d(1, 0, 0), 0, osg::Vec3d(0, 1, 0), 0, osg::Vec3d(0, 0, 1))));
		positioned->postMult(osg::Matrix::translate(position));
		
		
		positioned->addChild(glider);

		osg::PositionAttitudeTransform* xform = new osg::PositionAttitudeTransform;
		xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath,0.0,1.0));
		xform->addChild(positioned);

		model->addChild(xform);
		
//	}

	global->getRoot()->addChild(model);


// 	osgEarth::Annotation::LocalGeometryNode *ModelNode = new osgEarth::Annotation::LocalGeometryNode(global->getMapNode(), glider);
// 	ModelNode->setPosition(osgEarth::GeoPoint(global->getMap3D()->getSRS(), 10, 10, 1000, osgEarth::ALTMODE_RELATIVE));
// 	ModelNode->setScale(osg::Vec3f(10000, 10000, 10000));
// 	global->getRoot()->addChild(ModelNode);
	osgEarth::Viewpoint viewpoint("tt", 10, 10, 0, 0, -21.6, 80000);
	//viewpoint.setNode(ModelNode);
	global->getManipulator()->setViewpoint(viewpoint, 3);
}

Q_EXPORT_PLUGIN2(DrawEditor, DrawEditor)
