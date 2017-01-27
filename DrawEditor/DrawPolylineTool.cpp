#include "DrawPolylineTool.h"
#include "component/GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "Polyline.h"
#include "GraphicLayer.h"
#include "osgEarthSymbology\Geometry"
#include "osgEarthFeatures\Feature"
#include "draweditor_global.h"
#include "osgEarthAnnotation\FeatureNode"


DrawPolylineTool::DrawPolylineTool(QgsMapCanvas *map)
{
	m_rubberBand = new QgsRubberBand(map, QGis::Line);
	if(map) map->scene()->addItem(m_rubberBand);
	m_rubberBand->setColor(Qt::gray);
	m_rubberBand->setWidth(2);
}


DrawPolylineTool::~DrawPolylineTool(void)
{
	delete m_rubberBand;
}

void DrawPolylineTool::canvasMoveEvent(QgsMapMouseEvent* e)
{
	m_rubberBand->movePoint(m_rubberBand->partSize(0)-1, e->mapPoint());
}

void DrawPolylineTool::canvasPressEvent(QgsMapMouseEvent* e)
{
	if(e->button() == Qt::LeftButton){
		m_rubberBand->addPoint(e->mapPoint());
		m_points << QgsPoint2Vec3d(e->mapPoint(), 10000);
	}
}

void DrawPolylineTool::canvasReleaseEvent(QgsMapMouseEvent* e)
{
	if(e->button() == Qt::RightButton){
		GraphicLayer *layer = getActiveLayer();
		if(layer == 0) return;
		Polyline *gra = layer->createGraphic<Polyline>();
		gra->setVertexs(m_points);

		m_points.clear();
		m_rubberBand->reset(QGis::Line);
		layer->clearCacheImage();
	}
}
