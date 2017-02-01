#pragma once

#include "qmath.h"
#include "QObject"
#include "QColor"
#include "qgspoint.h"
#include "QList"
#include "qgsmapmouseevent.h"
#include "intersection/intersection.h"
#include "osg\Vec3d"
#include "osgEarthAnnotation\FeatureNode"

class GraphicEditor;
class GraphicLayer;
class GraphicItem : public QObject
{
	Q_OBJECT
public:
	enum GraphicType{
		None,
		Point,
		Polyline,
		Polygon
	};

	GraphicItem(GraphicType type);
	~GraphicItem(void);

	//properties
	GraphicType getGraphicType() const { return m_graphicType; }
	void setGraphicType(GraphicType type);

	QColor getBorderColor() const { return m_borderColor; }
	void setBorderColor(QColor color);

	int getBorderWidth() const { return m_borderWidth; }
	void setBorderWidth(int width);

	Qt::PenStyle getBorderStyle() const { return m_borderStyle; }
	void setBorderStyle(Qt::PenStyle style);

	bool getIsOutline() const { return m_isOutline; }
	void setIsOutline(bool outline) { m_isOutline = outline; }

	QColor getOutlineColor() const { return m_outLineColor; }
	void setOutlineColor(QColor color) { m_outLineColor = color; }

	int getOutlineWidth() const { return m_outLineWidth; }
	void setOutlineWidth(int width) { m_outLineWidth = width; }

	QColor getFillColor() const { return m_fillColor; }
	void setFillColor(QColor color);

	QString getIconPath() const { return m_iconPath; }
	void setIconPath(QString path);

	QString getID() const { return m_ID; }
	void setID(QString ID) { m_ID = ID; }

	int getIconWidth() const { return m_iconWidth; }
	void setIconWidth(int width);

	int getIconHeight() const { return m_iconHeight; }
	void setIconHeight(int height) { m_iconHeight = height; }

	GraphicEditor *getEditor() const { return m_editor; }
	void setEditor(GraphicEditor *editor) { m_editor = editor; }

	QImage getImage() const { return m_cache; }
	void setImage(QImage image) { m_cache = image; }

	GraphicLayer *getLayer() const { return m_layer; }
	void setLayer(GraphicLayer *layer) { m_layer = layer; }

	intersection2::Result getSelectResult() const { return m_res; }
	void setSelectResult(intersection2::Result res) { m_res = res; }

// 	//virtual
// 	virtual QgsPoint getVertex() { return QgsPoint(); }
// 	virtual QList<QgsPoint> getVertexs() { return QList<QgsPoint>(); }
// 	virtual void setVertex(QgsPoint pt) {}
// 	virtual void setVertexs(QList<QgsPoint> pts) {}

	QVector<osg::Vec3d> getVertexs() const { return m_vertexs; }
	void setVertexs(QVector<osg::Vec3d> vertexs);
	void setVertex(osg::Vec3d vertex) { setVertexs(QVector<osg::Vec3d>() << vertex); }

	osgEarth::Annotation::FeatureNode *getFeatureNode() const { return m_featureNode; }
	void setFeatureNode(osgEarth::Annotation::FeatureNode *node) { m_featureNode = node; }

	virtual bool isSelect(QgsMapMouseEvent *event) { return false; }

private:
	QString m_ID;
	GraphicType m_graphicType;
	QColor m_borderColor;
	int m_borderWidth;
	Qt::PenStyle m_borderStyle;
	bool m_isOutline;
	QColor m_outLineColor;
	int m_outLineWidth;
	QColor m_fillColor;
	QString m_iconPath;
	QImage m_cache;
	int m_iconWidth;
	int m_iconHeight;
	GraphicEditor *m_editor;
	GraphicLayer *m_layer;
	intersection2::Result m_res;
	QVector<osg::Vec3d> m_vertexs;
	osgEarth::Annotation::FeatureNode *m_featureNode;
};

