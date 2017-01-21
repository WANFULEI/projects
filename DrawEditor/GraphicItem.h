#pragma once
#include "QObject"
#include "QColor"
#include "qgspoint.h"
#include "QList"
#include "qgsmapmouseevent.h"

class GraphicEditor;
class GraphicItem : public QObject
{
	Q_OBJECT
public:
	GraphicItem(void);
	~GraphicItem(void);

	enum GraphicType{
		None,
		Point,
		Polyline,
		Polygon
	};

	//properties
	GraphicType getGraphicType() const { return m_graphicType; }
	void setGraphicType(GraphicType type) { m_graphicType = type; }

	QColor getBorderColor() const { return m_borderColor; }
	void setBorderColor(QColor color) { m_borderColor = color; }

	int getBorderWidth() const { return m_borderWidth; }
	void setBorderWidth(int width) { m_borderWidth = width; }

	Qt::PenStyle getBorderStyle() const { return m_borderStyle; }
	void setBorderWidth(Qt::PenStyle style) { m_borderStyle = style; }

	bool getIsOutline() const { return m_isOutline; }
	void setIsOutline(bool outline) { m_isOutline = outline; }

	QColor getOutlineColor() const { return m_outLineColor; }
	void setOutlineColor(QColor color) { m_outLineColor = color; }

	int getOutlineWidth() const { return m_outLineWidth; }
	void setOutlineWidth(int width) { m_outLineWidth = width; }

	QColor getFillColor() const { return m_fillColor; }
	void setFillColor(QColor color) { m_fillColor = color; }

	QString getIconPath() const { return m_iconPath; }
	void setIconPath(QString path) { m_iconPath = path; }

	QString getID() const { return m_ID; }
	void setID(QString ID) { m_ID = ID; }

	int getIconWidth() const { return m_iconWidth; }
	void setIconWidth(int width) { m_iconWidth = width; }

	int getIconHeight() const { return m_iconHeight; }
	void setIconHeight(int height) { m_iconHeight = height; }

	GraphicEditor *getEditor() const { return m_editor; }
	void setEditor(GraphicEditor *editor) { m_editor = editor; }

	QImage getImage() const { return m_cache; }
	void setImage(QImage image) { m_cache = image; }

	//virtual
	virtual QgsPoint getVertex() { return QgsPoint(); }
	virtual QList<QgsPoint> getVertexs() { return QList<QgsPoint>(); }
	virtual void setVertex(QgsPoint pt) {}
	virtual void setVertexs(QList<QgsPoint> pts) {}

	bool isSelect(QgsMapMouseEvent *event) { return false; }

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
};

