#pragma once

#include "../../control/layerwidget/Qt_widget_layer.h"
#include <QColor>
#include <QBrush>

class OGRFeature;
class OGRGeometry;
class vector_layer : public layerwidget::Qt_widget_layer
{
public:
	vector_layer(void);
	~vector_layer(void);

	bool load(const QString & url);

	void update_envelope();
	QRectF get_envelope() const { return m_envelope; }

	void free_features();

protected:
	void draw();

	void draw_polygon(OGRGeometry * geometry, QPainter &pa);

private:
	QString m_url;
	QList<OGRFeature *> m_features;
	QRectF m_envelope;
	QColor m_border_color;
	QBrush m_fill_brush;
	double m_border_width;
	Qt::PenStyle m_border_style;
	Qt::PenCapStyle m_border_cap;
	Qt::PenJoinStyle m_border_join;
};

