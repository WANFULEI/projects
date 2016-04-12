#include "Qt_widget2.h"
#include "Qt_gird_layer.h"
#include <QtCore/QDebug>
#include <windows.h>

namespace layerwidget{

Qt_widget2::Qt_widget2(QWidget * parent)
	:Qt_widget(parent)
{
	m_image_layer = new Qt_image_layer;
	attach(m_image_layer);
	attach(new Qt_gird_layer);
	m_geometry_layer = Qt_geometry_layer_manager::get_instance()->create_entry();
	attach(m_geometry_layer);
	m_select_tool = new Qt_geometry_interaction_layer;
	attach(m_select_tool);
	m_pan_tool = new Qt_pan_layer;
	attach(m_pan_tool);
	m_select_tool->add_geometry_layer(m_geometry_layer);

	select_tool(Select);
	m_is_select = true;
}


Qt_widget2::~Qt_widget2(void)
{
}

void Qt_widget2::keyPressEvent(QKeyEvent *e)
{
	__super::keyPressEvent(e);
	if (e->key() == Qt::Key_Space)
	{
		if (get_tool() == Select)
		{
			select_tool(Pan);
			m_is_select = true;
		}
	}
}

void Qt_widget2::keyReleaseEvent(QKeyEvent *e)
{
	__super::keyReleaseEvent(e);
	if (e->key() == Qt::Key_Space)
	{
		if(m_is_select)
		{
			select_tool(Select);
		}
	}
}

void Qt_widget2::select_tool(ToolType type)
{
	if (type == Select)
	{
		m_pan_tool->deactivate();
		m_select_tool->activate();
		setCursor(Qt::ArrowCursor);
		m_is_select = true;
	}
	else if (type == Pan)
	{
		m_select_tool->deactivate();
		m_pan_tool->activate();
		if (GetAsyncKeyState(VK_LBUTTON) < 0)
		{
			setCursor(Qt::ClosedHandCursor);
		}
		else
		{
			setCursor(Qt::OpenHandCursor);
		}
		m_is_select = false;
	}
	else
	{

	}
}

layerwidget::ToolType Qt_widget2::get_tool()
{
	if (m_pan_tool->is_active())
	{
		return Pan;
	}
	else if (m_select_tool->is_active())
	{
		return Select;
	}
	else
	{

	}
	return Select;
}

}