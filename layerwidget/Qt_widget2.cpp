#include "Qt_widget2.h"
#include "Qt_gird_layer.h"

namespace layerwidget{

Qt_widget2::Qt_widget2(QWidget * parent)
	:Qt_widget(parent)
{
	attach(new Qt_gird_layer);
	m_geometry_layer = new Qt_geometry_layer;
	attach(m_geometry_layer);
	m_select_tool = new Qt_geometry_interaction_layer;
	attach(m_select_tool);
	m_pan_tool = new Qt_pan_layer;
	attach(m_pan_tool);
	m_select_tool->add_geometry_layer(m_geometry_layer);

	select_tool(Select);
}


Qt_widget2::~Qt_widget2(void)
{
}

void Qt_widget2::select_tool(ToolType type)
{
	if (type == Select)
	{
		m_pan_tool->deactivate();
		m_select_tool->activate();
	}
	else if (type == Pan)
	{
		m_select_tool->deactivate();
		m_pan_tool->activate();
	}
	else
	{

	}
}

}