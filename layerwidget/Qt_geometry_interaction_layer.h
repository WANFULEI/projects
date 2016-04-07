#pragma once
#include "qt_tools_layer.h"
#include "layerwidget_global.h"
#include "Qt_geometry_layer.h"
#include "../intersection/intersection.h"
#include <QtCore/QList>

namespace layerwidget
{

class LAYERWIDGET_EXPORT Qt_geometry_interaction_layer :
	public Qt_select_layer
{
public:
	Qt_geometry_interaction_layer(void);
	~Qt_geometry_interaction_layer(void);

	void add_geometry_layer(Qt_geometry_layer * layer);

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

	void is_select(const QPoint & pt);
	intersection2::Result is_select(const Feature & geo,const QPoint & pt);

	QList<Qt_geometry_layer *> m_geometry_layers;
	Geometry * m_geometry;
	intersection2::Result m_res;
	QPoint m_pt_down;
};

}