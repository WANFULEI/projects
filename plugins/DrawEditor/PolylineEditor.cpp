#include "PolylineEditor.h"
#include "GraphicItem.h"
#include "component/GlobalInstance.h"
#include "GraphicLayer.h"
#include "qgsmapcanvas.h"
#include "draweditor_global.h"

PolylineEditor::PolylineEditor(void)
{
	//m_highlight = 0;
	m_rubberBand = 0;
}


PolylineEditor::~PolylineEditor(void)
{
	//delete m_highlight;
}

void PolylineEditor::canvasMoveEvent(QgsMapMouseEvent* e)
{
	if(getEditingItem() == 0) return;
	if((e->pos() - m_ptDown).manhattanLength() < qApp->startDragDistance()) return;
	if(e->buttons() & Qt::LeftButton){
		QVector<osg::Vec3d> points = getEditingItem()->getVertexs();
		auto res = getEditingItem()->getSelectResult();
		if(res.type == intersection2::Point){
			if(res.index < 0 || res.index >= points.size()) return;
			points[res.index] = QgsPoint2Vec3d(e->mapPoint());
			if(m_rubberBand == 0){
				m_rubberBand = new QgsRubberBand(global->getMap2D(), QGis::Line);
				global->getMap2D()->scene()->addItem(m_rubberBand);
				m_rubberBand->setBorderColor(Qt::gray);
				m_rubberBand->setWidth(2);
			}
			m_rubberBand->reset(QGis::Line);
			if(res.index - 1 >= 0){
				m_rubberBand->addPoint(Vec3d2QgsPoint(points[res.index-1]));
			}
			m_rubberBand->addPoint(e->mapPoint());
			if(res.index + 1 < points.size()){
				m_rubberBand->addPoint(Vec3d2QgsPoint(points[res.index+1]));
			}
		}else if(res.type == intersection2::Line){
			if(res.index < 0 || res.index >= points.size()-1) return;
			//points[res.index] = QgsPoint2Vec3d(e->mapPoint());
			if(m_rubberBand == 0){
				m_rubberBand = new QgsRubberBand(global->getMap2D(), QGis::Line);
				global->getMap2D()->scene()->addItem(m_rubberBand);
				m_rubberBand->setBorderColor(Qt::gray);
				m_rubberBand->setWidth(2);
			}
			m_rubberBand->reset(QGis::Line);
			if(res.index >= 0){
				m_rubberBand->addPoint(Vec3d2QgsPoint(points[res.index]));
			}
			m_rubberBand->addPoint(e->mapPoint());
			if(res.index + 1 < points.size()){
				m_rubberBand->addPoint(Vec3d2QgsPoint(points[res.index+1]));
			}
		}
	}
}

void PolylineEditor::canvasPressEvent(QgsMapMouseEvent* e)
{
	highlight();
	m_ptDown = e->pos();
	return;	return;
}

void PolylineEditor::canvasReleaseEvent(QgsMapMouseEvent* e)
{
	if(getEditingItem() == 0) return;
	if((e->pos() - m_ptDown).manhattanLength() < qApp->startDragDistance()) return;
	QVector<osg::Vec3d> points = getEditingItem()->getVertexs();
	auto res = getEditingItem()->getSelectResult();
	if(res.type == intersection2::Point){
		if(res.index < 0 || res.index >= points.size()) return;
		points[res.index] = QgsPoint2Vec3d(e->mapPoint());
	}else if(res.type == intersection2::Line){
		points.insert(res.index+1, QgsPoint2Vec3d(e->mapPoint()));
		res.index++;
	}
	getEditingItem()->setVertexs(points);
	getEditingItem()->getLayer()->clearCacheImage();
	highlight();

	global->getMap2D()->scene()->removeItem(m_rubberBand);
	delete m_rubberBand;
	m_rubberBand = 0;
}

void PolylineEditor::highlight()
{
	if(getEditingItem() == 0) return;
// 	if(m_highlight){
// 		global->getMap2D()->scene()->removeItem(m_highlight);
// 		delete m_highlight;
// 		m_highlight = 0;
// 	}
	auto points = getEditingItem()->getVertexs();
	while(m_markers.size() < points.size()){
		m_markers << new QgsVertexMarker(global->getMap2D());
		global->getMap2D()->scene()->addItem(m_markers.last());
	}
	while(m_markers.size() > points.size()){
		global->getMap2D()->scene()->removeItem(m_markers.first());
		delete m_markers.first();
		m_markers.removeFirst();
	}
	for(int i=0; i<points.size(); ++i){
		if(i == 0){
			m_markers[i]->setColor(Qt::green);
		}else if(getEditingItem()->getSelectResult().type == intersection2::Point && i == getEditingItem()->getSelectResult().index){
			m_markers[i]->setColor(Qt::blue);
		}else{
			m_markers[i]->setColor(Qt::white);
		}
		m_markers[i]->setPenWidth(2);
		m_markers[i]->setIconType(QgsVertexMarker::ICON_BOX);
		m_markers[i]->setIconSize(8);
		m_markers[i]->setCenter(Vec3d2QgsPoint(points[i]));
		m_markers[i]->update(m_markers[i]->boundingRect());
	}
// 	QgsGeometry *geom = QgsGeometry::fromPolyline(points);
// 	m_highlight = new QgsHighlight(global->getMap2D(), geom, getEditingItem()->getLayer());
// 	m_highlight->setColor(Qt::green);
// 	m_highlight->setWidth(getEditingItem()->getBorderWidth() + getEditingItem()->getIsOutline() ? 2 * getEditingItem()->getOutlineWidth() : 0);
// 	global->getMap2D()->scene()->addItem(m_highlight);
}

void PolylineEditor::onDeActive()
{
	foreach(QgsVertexMarker *marker, m_markers){
		global->getMap2D()->scene()->removeItem(marker);
		delete marker;
	}
	m_markers.clear();
}
