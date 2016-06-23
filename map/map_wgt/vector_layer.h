#pragma once

#include <QColor>
#include <QBrush>
#include "..\..\control\layer_widget\layer.h"
#include "map_wgt_global.h"
#include <QMap>
#include <QPair>

class OGRFeature;
class OGRGeometry;

namespace map_wgt{

class MAP_WGT_EXPORT vector_layer : public layer_wgt::layer
{
public:
	vector_layer(void);
	~vector_layer(void);

	bool load(const QString & url);

	void update_envelope();
	QRectF get_envelope() const { return m_envelope; }

	void free_features();

protected:
	void gl_draw();

	void draw_polygon2(OGRGeometry * geometry);

	bool is_in_viewport(OGRGeometry * geometry, QRectF &viewport);

	void draw_polygon(OGRGeometry * geometry);

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
	QMap<OGRGeometry *, QPair<int, int>> m_tmp;
};
}