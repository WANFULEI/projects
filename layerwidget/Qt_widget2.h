#pragma once
#include "qt_widget.h"
#include "Qt_tools_layer.h"
#include "Qt_geometry_layer.h"
#include "layerwidget_global.h"
#include "Qt_geometry_interaction_layer.h"

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

	void select_tool(ToolType type);
	Qt_geometry_layer * get_geometry_layer() const { return m_geometry_layer; }
	
	Qt_geometry_layer * m_geometry_layer;
	Qt_pan_layer * m_pan_tool;
	Qt_geometry_interaction_layer * m_select_tool;
};

}