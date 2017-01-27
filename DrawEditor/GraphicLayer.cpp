#include "GraphicLayer.h"
#include "GraphicItem.h"
#include "QPainter"
#include "component/GlobalInstance.h"
#include "draweditor_global.h"


GraphicLayer::GraphicLayer(QString name)
	:QgsPluginLayer("plugin", name)
{
	setValid(true);
	m_group = new osg::Group;
	global->getRoot()->addChild(m_group);
}


GraphicLayer::~GraphicLayer(void)
{

}

void GraphicLayer::addGraphic(GraphicItem *gra)
{
	if(gra == 0 || m_graphics.contains(gra)) return;
	gra->setLayer(this);
	m_graphics << gra;
	m_group->addChild(gra->getFeatureNode());
}

void GraphicLayer::removeGraphic(GraphicItem *gra)
{
	m_graphics.removeOne(gra);
	m_group->removeChild(gra->getFeatureNode());
	delete gra;
}

void GraphicLayer::removeGraphic(QString ID)
{
	removeGraphic(getGraphic(ID));
}

GraphicItem * GraphicLayer::getGraphic(QString ID)
{
	for(auto i = m_graphics.begin(); i != m_graphics.end(); ++i){
		if((*i)->getID() == ID) return *i;
	}
	return 0;
}

bool GraphicLayer::draw(QgsRenderContext& rendererContext)
{
	QPainter *p = rendererContext.painter();
	p->save();
	QList<GraphicItem *> graphics = m_graphics;
	for(auto i = graphics.begin(); i != graphics.end(); ++i){
		GraphicItem *gra = *i;
		if(gra == 0) continue;
		try{
			QObject *obj = dynamic_cast<QObject *>(gra);
		}catch(...){
			continue;
		}
		QVector<osg::Vec3d> vertexs = gra->getVertexs();
		switch(gra->getGraphicType()){
		case GraphicItem::Point:
			{
				if(vertexs.size() < 1) continue;
				QgsPoint pt = rendererContext.mapToPixel().transform(Vec3d2QgsPoint(vertexs[0]));
				QRect rc = QRect(pt.x()-gra->getIconWidth()/2, pt.y()-gra->getIconHeight()/2, gra->getIconWidth(), gra->getIconHeight());
				if(gra->getImage().isNull()){
					QImage image;
					image.load(gra->getIconPath());
					p->drawImage(rc, image);
					gra->setImage(image);
				}else{
					p->drawImage(rc, gra->getImage());
				}
				break;
			}
		case GraphicItem::Polyline:
			{
				QVector<QPoint> points(vertexs.size());
				int j=0;
				for(auto i = vertexs.begin(); i != vertexs.end(); ++i, ++j){
					QgsPoint pt = rendererContext.mapToPixel().transform(Vec3d2QgsPoint(*i));
					points[j] = QPoint(pt.x(), pt.y());
				}
				if(gra->getIsOutline()){
					p->setPen(QPen(gra->getOutlineColor(), gra->getBorderWidth() + gra->getOutlineWidth() * 2, Qt::SolidLine));
					p->drawPolyline(points);
				}
				p->setPen(QPen(gra->getBorderColor(), gra->getBorderWidth(), gra->getBorderStyle()));
				p->drawPolyline(points);
				break;
			}
		case GraphicItem::Polygon:
			{
				if(vertexs.size() < 3) continue;
				QVector<QPoint> points(vertexs.size());
				int j=0;
				for(auto i = vertexs.begin(); i != vertexs.end(); ++i, ++j){
					QgsPoint pt = rendererContext.mapToPixel().transform(Vec3d2QgsPoint(*i));
					points[j] = QPoint(pt.x(), pt.y());
				}
				if(gra->getIsOutline()){
					p->setPen(QPen(gra->getBorderColor(), 0, gra->getBorderStyle()));
					p->setBrush(QBrush(gra->getFillColor()));
					p->drawPolygon(points);
					p->setPen(QPen(gra->getOutlineColor(), gra->getBorderWidth() + gra->getOutlineWidth() * 2, Qt::SolidLine));
					p->drawPolyline(points << points[0]);
					p->setPen(QPen(gra->getBorderColor(), gra->getBorderWidth(), gra->getBorderStyle()));
					p->drawPolyline(points << points[0]);
				}else{
					p->setPen(QPen(gra->getBorderColor(), gra->getBorderWidth(), gra->getBorderStyle()));
					p->setBrush(QBrush(gra->getFillColor()));
					p->drawPolygon(points);
				}
				break;
			}
		default:break;
		}
	}
	p->restore();
	return true;
}

void GraphicLayer::setAbstract(const QString& abstract)
{
	
}

QString GraphicLayer::abstract() const
{
	return QString();
}

bool GraphicLayer::readSymbology(const QDomNode& node, QString& errorMessage)
{
	return false;
}

bool GraphicLayer::writeSymbology(QDomNode &node, QDomDocument& doc, QString& errorMessage) const
{
	return false;
}
