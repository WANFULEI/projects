#include "GraphicLayer.h"
#include "GraphicItem.h"
#include "QPainter"


GraphicLayer::GraphicLayer(QString name)
	:QgsPluginLayer("plugin", name)
{
	mValid = true;
}


GraphicLayer::~GraphicLayer(void)
{

}

void GraphicLayer::addGraphic(GraphicItem *gra)
{
	if(gra == 0 || m_graphics.contains(gra)) return;
	m_graphics << gra;
}

void GraphicLayer::removeGraphic(GraphicItem *gra)
{
	m_graphics.removeOne(gra);
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
	for(auto i = m_graphics.begin(); i != m_graphics.end(); ++i){
		GraphicItem *gra = *i;
		if(gra == 0) continue;
		switch(gra->getGraphicType()){
		case GraphicItem::Point:
			{
				QgsPoint pt = rendererContext.mapToPixel().transform(gra->getVertex());
				QRect rc = QRect(pt.x()-gra->getIconWidth()/2, pt.y()-gra->getIconHeight()/2, gra->getIconWidth(), gra->getIconHeight());
				if(gra->getImage().isNull()){
					QImage image;
					image.load(gra->getIconPath());
					p->drawImage(rc, image);
				}else{
					p->drawImage(rc, gra->getImage());
				}
				break;
			}
		case GraphicItem::Polyline:
			{
				QList<QgsPoint> vertexs = gra->getVertexs();
				QVector<QPoint> points(vertexs.size());
				int j=0;
				for(auto i = vertexs.begin(); i != vertexs.end(); ++i, ++j){
					QgsPoint pt = rendererContext.mapToPixel().transform(*i);
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
				QList<QgsPoint> vertexs = gra->getVertexs();
				QVector<QPoint> points(vertexs.size());
				int j=0;
				for(auto i = vertexs.begin(); i != vertexs.end(); ++i, ++j){
					QgsPoint pt = rendererContext.mapToPixel().transform(*i);
					points[j] = QPoint(pt.x(), pt.y());
				}
				if(gra->getIsOutline()){
					p->setPen(QPen(gra->getOutlineColor(), gra->getBorderWidth() + gra->getOutlineWidth() * 2, Qt::SolidLine));
					p->drawPolyline(points);
				}
				p->setPen(QPen(gra->getBorderColor(), gra->getBorderWidth(), gra->getBorderStyle()));
				p->setBrush(QBrush(gra->getFillColor()));
				p->drawPolygon(points);
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
