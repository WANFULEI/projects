#pragma once
#include "qt_tools_layer.h"
#include "layerwidget_global.h"
#include "Qt_geometry_layer.h"
#include "intersection/intersection.h"
#include <QtCore/QList>
#include "baseset/share_obj.h"

namespace layerwidget
{

class LAYERWIDGET_EXPORT Qt_geometry_interaction_layer :
	public Qt_select_layer
{
public:
	Qt_geometry_interaction_layer(void);
	~Qt_geometry_interaction_layer(void);

	void add_geometry_layer(const share_ptr<Qt_geometry_layer> & layer);

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

	void is_select(const QPoint & pt);
	intersection2::Result is_select(const share_ptr<Feature> & geo,const QPoint & pt);

	share_list_vector_manager<Qt_geometry_layer> m_geometry_layers;
	share_ptr<Feature> m_feature;
	intersection2::Result m_res;
	QPoint m_pt_down;
};

}