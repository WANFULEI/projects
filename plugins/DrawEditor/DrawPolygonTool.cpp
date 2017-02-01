#include "DrawPolygonTool.h"
#include "qgsrubberband.h"
#include "Polygon.h"
#include "GraphicLayer.h"
#include "qgsmapcanvas.h"
#include "osgEarthSymbology\Geometry"
#include "osgEarthFeatures\Feature"
#include "component/GlobalInstance.h"
#include "osgEarth\Map"
#include "osgEarthSymbology\Style"
#include <osgEarthAnnotation/FeatureNode>
#include "draweditor_global.h"

DrawPolygonTool::DrawPolygonTool(QgsMapCanvas *map)
{
	m_rubberBand = new QgsRubberBand(map, QGis::Polygon);
	if(map) map->scene()->addItem(m_rubberBand);
	m_rubberBand->setColor(Qt::gray);
	m_rubberBand->setFillColor(Qt::transparent);
	m_rubberBand->setWidth(2);
	m_rubberBand->closePoints(false);
}


DrawPolygonTool::~DrawPolygonTool(void)
{
}

void DrawPolygonTool::canvasMoveEvent(QgsMapMouseEvent* e)
{
	m_rubberBand->movePoint(m_rubberBand->partSize(0)-1, e->mapPoint());
}

void DrawPolygonTool::canvasPressEvent(QgsMapMouseEvent* e)
{
	if(e->button() == Qt::LeftButton){
		m_rubberBand->addPoint(e->mapPoint());
		m_points << QgsPoint2Vec3d(e->mapPoint());
	}
}

void DrawPolygonTool::canvasReleaseEvent(QgsMapMouseEvent* e)
{
	if(e->button() == Qt::RightButton){
		GraphicLayer *layer = getActiveLayer();
		if(layer == 0) return;
		Polygon *gra = layer->createGraphic<Polygon>();
		gra->setVertexs(m_points);

		m_rubberBand->reset(QGis::Polygon);
		layer->clearCacheImage();
		m_points.clear();
	}
}
