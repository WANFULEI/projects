#include "GraphicItem.h"
#include "QUuid"
#include "component/GlobalInstance.h"
#include "osgEarth\Map"
#include "draweditor_global.h"
#include "osgEarthAnnotation/LocalGeometryNode"
#include "osgEarthAnnotation/ModelNode"

GraphicItem::GraphicItem(GraphicType type)
{
	m_graphicType = None;
	m_borderColor = Qt::red;
	m_borderWidth = 2;
	m_borderStyle = Qt::SolidLine;
	m_isOutline = true;
	m_outLineColor = Qt::black;
	m_outLineWidth = 1;
	m_fillColor = QColor(255, 255, 0, 128);
	m_ID = QUuid::createUuid().toString();
	m_iconWidth = 24;
	m_iconHeight = 24;
	m_editor = 0;
	m_layer = 0;
	m_featureNode = 0;
	setGraphicType(type);
}


GraphicItem::~GraphicItem(void)
{
	delete m_featureNode;
}

void GraphicItem::setGraphicType(GraphicType type)
{
	m_graphicType = type;
	osgEarth::Symbology::Geometry *geom = 0;
	osgEarth::Symbology::Style geomStyle;
	if(type == Point){
		geom = new osgEarth::Symbology::PointSet;
		geomStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->setImage(osgDB::readImageFile(m_iconPath.toStdString()));
	}else if(type == Polyline){
		geom = new osgEarth::Symbology::LineString;
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = QColor2OsgEarthColor(m_borderColor);
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = m_borderWidth;
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellationSize() = 75000;
	}else if(type == Polygon){
		geom = new osgEarth::Symbology::Polygon;
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = QColor2OsgEarthColor(m_borderColor);
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = m_borderWidth;
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellationSize() = 75000;
		geomStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill() = QColor2OsgEarthColor(m_fillColor);
	}
	if(geom == 0) return;
	for(int i=0; i<m_vertexs.size(); ++i){
		geom->push_back(m_vertexs[i]);
	}
	osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(geom, global->getMap3D()->getSRS());
	feature->geoInterp() = osgEarth::GEOINTERP_GREAT_CIRCLE;
	m_featureNode = new osgEarth::Annotation::FeatureNode(global->getMapNode(), feature, geomStyle);
}

void GraphicItem::setVertexs(QVector<osg::Vec3d> vertexs)
{
	m_vertexs = vertexs;
	if(m_featureNode){
		auto geom = m_featureNode->getFeature()->getGeometry();
		geom->assign(vertexs.begin(), vertexs.end());
		m_featureNode->init();
	}
}

void GraphicItem::setBorderColor(QColor color)
{
	m_borderColor = color;
	if(m_featureNode){
		auto geomStyle = m_featureNode->getStyle();
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = QColor2OsgEarthColor(m_borderColor);
		m_featureNode->setStyle(geomStyle);
		m_featureNode->init();
	}
}

void GraphicItem::setBorderWidth(int width)
{
	m_borderWidth = width;
	if(m_featureNode){
		auto geomStyle = m_featureNode->getStyle();
		geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = m_borderWidth;
		m_featureNode->setStyle(geomStyle);
		m_featureNode->init();
	}
}

void GraphicItem::setBorderStyle(Qt::PenStyle style)
{
	m_borderStyle = style;
}

void GraphicItem::setFillColor(QColor color)
{
	m_fillColor = color;
	if(m_featureNode){
		auto geomStyle = m_featureNode->getStyle();
		geomStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill() = QColor2OsgEarthColor(m_fillColor);
		m_featureNode->setStyle(geomStyle);
		m_featureNode->init();
	}
}

void GraphicItem::setIconPath(QString path)
{
	m_iconPath = path;
	if(m_featureNode){
		auto geomStyle = m_featureNode->getStyle();
		geomStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->setImage(osgDB::readImageFile(m_iconPath.toStdString()));
		m_featureNode->setStyle(geomStyle);
		m_featureNode->init();
	}
}

void GraphicItem::setIconWidth(int width)
{
	m_iconWidth = width;
}
