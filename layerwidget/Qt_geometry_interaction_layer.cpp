#include "Qt_geometry_interaction_layer.h"
#include "Qt_widget.h"

namespace layerwidget
{

Qt_geometry_interaction_layer::Qt_geometry_interaction_layer(void)
{
	m_feature = 0;
}


Qt_geometry_interaction_layer::~Qt_geometry_interaction_layer(void)
{
}

void Qt_geometry_interaction_layer::add_geometry_layer(Qt_geometry_layer * layer)
{
	if (layer == 0)
	{
		return;
	}
	m_geometry_layers << layer;
}

void Qt_geometry_interaction_layer::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_pt_down = event->pos();
		is_select(event->pos());
	}
}


void Qt_geometry_interaction_layer::mouseMoveEvent(QMouseEvent * event)
{

	if(event->buttons() & Qt::LeftButton)
	{
		if (m_res.type != intersection2::None)
		{
			if (m_feature == 0)
			{
				return;
			}
			if (m_feature->geo.type == Geometry::Point)
			{
				m_feature->geo.pt.rx() = widget->x_real(event->pos().x());
				m_feature->geo.pt.ry() = widget->y_real(event->pos().y());
			}
			else if (m_feature->geo.type == Geometry::Picture)
			{

			}
			else if (m_feature->geo.type == Geometry::Polyline)
			{

			}
			else if (m_feature->geo.type == Geometry::Ellipse)
			{

			}
			else if (m_feature->geo.type == Geometry::Rect)
			{

			}
			else if (m_feature->geo.type == Geometry::Polygon)
			{

			}
			else
			{

			}
			widget->redraw();
		}
	}
	__super::mouseMoveEvent(event);
}

void Qt_geometry_interaction_layer::mouseReleaseEvent(QMouseEvent * event)
{
	if (m_feature == 0)
	{
		return;
	}
	if(event->button() == Qt::LeftButton)
	{
		if (m_res.type != intersection2::None)
		{
			if (m_feature->geo.type == Geometry::Point)
			{
				m_feature->geo.pt.rx() = widget->x_real(event->pos().x());
				m_feature->geo.pt.ry() = widget->y_real(event->pos().y());
			}
			else if (m_feature->geo.type == Geometry::Picture)
			{

			}
			else if (m_feature->geo.type == Geometry::Polyline)
			{

			}
			else if (m_feature->geo.type == Geometry::Ellipse)
			{

			}
			else if (m_feature->geo.type == Geometry::Rect)
			{

			}
			else if (m_feature->geo.type == Geometry::Polygon)
			{

			}
			else
			{

			}
			widget->redraw();
		}
	}
}

intersection2::Result Qt_geometry_interaction_layer::is_select(Feature * feature,const QPoint & pt)
{
	intersection2::Result res;
	if (feature->geo.type == Geometry::Point)
	{
		int x = widget->x_pixel(feature->geo.pt.x());
		int y = widget->y_pixel(feature->geo.pt.y());
		if(intersection2::intersection(QPoint(x,y),pt))
		{
			res.type = intersection2::Point;
			res.index = 0;
		}
	}
	else if (feature->geo.type == Geometry::Picture)
	{

	}
	else if (feature->geo.type == Geometry::Polyline)
	{

	}
	else if (feature->geo.type == Geometry::Ellipse)
	{

	}
	else if (feature->geo.type == Geometry::Rect)
	{

	}
	else if (feature->geo.type == Geometry::Polygon)
	{

	}
	else
	{

	}
	return res;
}

void Qt_geometry_interaction_layer::is_select(const QPoint & pt)
{
	for (int i=0;i<m_geometry_layers.size();++i)
	{
		auto & geometries = m_geometry_layers[i]->get_geometries();
		auto iter = geometries.begin();
		while (iter != geometries.end())
		{
			m_res = is_select(iter.value(),pt);
			if (m_res.type != intersection2::None)
			{
				m_feature = iter.value();
				return;
			}
			++iter;
		}
	}
	m_feature = 0;
	m_res = intersection2::Result();
}

}