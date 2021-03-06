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
class LAYERWIDGET_EXPORT Qt_default_widget :
	public Qt_widget
{
public:
	Qt_default_widget(QWidget * parent = 0);
	~Qt_default_widget(void);

	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	void select_tool(ToolType type);
	ToolType get_tool();
	share_ptr<Qt_geometry_layer> get_geometry_layer() const { return m_geometry_layer; }
	share_ptr<Qt_image_layer> get_image_layer() const { return m_image_layer; }
	share_ptr<Qt_geometry_layer_manager> get_geometry_layer_manager() const { return m_geometry_layer_manager; }
	
protected:
	share_ptr<Qt_geometry_layer> m_geometry_layer;
	share_ptr<Qt_pan_layer> m_pan_tool;
	share_ptr<Qt_geometry_interaction_layer> m_select_tool;
	share_ptr<Qt_image_layer> m_image_layer;
	share_ptr<Qt_geometry_layer_manager> m_geometry_layer_manager;
	bool m_is_select;
};

}