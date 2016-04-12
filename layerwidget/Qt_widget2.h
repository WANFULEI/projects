#pragma once
#include "qt_widget.h"
#include "Qt_tools_layer.h"
#include "Qt_geometry_layer.h"
#include "layerwidget_global.h"
#include "Qt_geometry_interaction_layer.h"
#include "Qt_gird_layer.h"

namespace layerwidget
{
	enum ToolType
	{
		Select,
		Pan
	};
class LAYERWIDGET_EXPORT Qt_widget2 :
	public Qt_widget
{
public:
	Qt_widget2(QWidget * parent = 0);
	~Qt_widget2(void);

	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	void select_tool(ToolType type);
	ToolType get_tool();
	Qt_geometry_layer * get_geometry_layer() const { return m_geometry_layer; }
	Qt_image_layer * get_image_layer() const { return m_image_layer; }
	
	Qt_geometry_layer * m_geometry_layer;
	Qt_pan_layer * m_pan_tool;
	Qt_geometry_interaction_layer * m_select_tool;
	bool m_is_select;
	Qt_image_layer * m_image_layer;
};

}