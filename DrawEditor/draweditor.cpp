#include "draweditor.h"
#include <QObject>
#include "GlobalInstance.h"
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

DrawEditor::DrawEditor()
{
	m_vps = 0;
	m_drawKeypointTool = 0;
	m_drawPolylineTool = 0;
}

DrawEditor::~DrawEditor()
{
	delete m_vps;
	delete m_drawKeypointTool;
	delete m_drawPolylineTool;
}

void DrawEditor::initialize()
{
	QgsVectorLayer *layer = new QgsVectorLayer("Point?crs=epsg:4326", "test", "memory");
	if(layer->isValid()){
		QgsMarkerSymbolLayerV2 *symbolLayer = new QgsSvgMarkerSymbolLayerV2(qApp->applicationDirPath() + "/../images/svg/accommodation/accommodation_alpinehut.svg");
		symbolLayer->setSize(6);
		QgsSimpleMarkerSymbolLayerV2 *simpleLayer = new QgsSimpleMarkerSymbolLayerV2;
		QgsMarkerSymbolV2 *symbol = new QgsMarkerSymbolV2(QgsSymbolLayerV2List() << simpleLayer << symbolLayer);
		QgsSingleSymbolRendererV2 *render = new QgsSingleSymbolRendererV2(symbol);
		layer->setRendererV2(render);
		layer->startEditing();
		QgsFeature feature;
		feature.setGeometry(QgsGeometry::fromPoint(QgsPoint(10, 10)));
		layer->addFeature(feature);
		layer->commitChanges();

		QgsMapLayerRegistry::instance()->addMapLayer(layer);
		GlobalInstance::getInstance()->getLayerTreeRoot()->insertLayer(0, layer);
	}
	layer = new QgsVectorLayer("polygon?crs=epsg:4326", "polygonLayer", "memory");
	if(layer->isValid()){
		QgsSimpleFillSymbolLayerV2 *fillLayer = new QgsSimpleFillSymbolLayerV2;
		QgsFillSymbolV2 *symbol = new QgsFillSymbolV2(QgsSymbolLayerV2List() << fillLayer);
		QgsSingleSymbolRendererV2 *render = new QgsSingleSymbolRendererV2(symbol);
		layer->setRendererV2(render);
		layer->startEditing();
		QgsFeature feature;
		feature.setGeometry(QgsGeometry::fromPolygon(QgsPolygon() << (QgsPolyline() << QgsPoint(0, 0) << QgsPoint(10, 0) << QgsPoint(5, 5))));
		layer->addFeature(feature);
		layer->commitChanges();

		QgsMapLayerRegistry::instance()->addMapLayer(layer);
		GlobalInstance::getInstance()->getLayerTreeRoot()->insertLayer(0, layer);
	}

	long srsid = GlobalInstance::getInstance()->getMap2D()->mapSettings().destinationCrs().srsid();
	long srid = GlobalInstance::getInstance()->getMap2D()->mapSettings().destinationCrs().postgisSrid();
	QString proj4 = GlobalInstance::getInstance()->getMap2D()->mapSettings().destinationCrs().toProj4();


	//use
	m_vps = new DrawEditorVps(GlobalInstance::getInstance()->getMap2D());

	GraphicLayer *graLayer = new GraphicLayer("graphicLayer");
	QgsMapLayerRegistry::instance()->addMapLayer(graLayer);
	GlobalInstance::getInstance()->getLayerTreeRoot()->insertLayer(0, graLayer);
	m_drawKeypointTool = new DrawKeypointTool("../svg", GlobalInstance::getInstance()->getMainWindow());
	m_drawPolylineTool = new DrawPolylineTool(global->getMap2D());
}

void DrawEditor::slotSelect()
{
	GlobalInstance::getInstance()->getMap2D()->setMapTool(m_vps);
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

Q_EXPORT_PLUGIN2(DrawEditor, DrawEditor)
