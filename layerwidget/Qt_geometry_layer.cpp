
#include "Qt_geometry_layer.h"
#include "..\..\layerwidget\Qt_widget.h"


namespace layerwidget
{
	Qt_geometry_layer::Qt_geometry_layer(void)
	{
		m_next_geometry_id = 1;
	}


	Qt_geometry_layer::~Qt_geometry_layer(void)
	{
	}


	int Qt_geometry_layer::get_next_geometry_id()
	{
		return m_next_geometry_id++;
	}

	void Qt_geometry_layer::add_points(const QList<QPointF> & pts, const QColor & color,int size)
	{
		for (int i=0;i<pts.size();++i)
		{
			Geometry geo;
			Style style;
			style.border_color = color;
			style.fill_color = color;
			geo.pt = pts[i];
			style.size = size;
			geo.type = Geometry::Point;
			add_geometry(geo,style);
		}
	}

	int Qt_geometry_layer::add_ellipse(const QPointF & pt, double length, double width, const Style & style /*= Style()*/)
	{
		Geometry geo;
		geo.pt = pt;
		geo.type = Geometry::Ellipse;
		geo.width = width;
		geo.length = length;
		
		return add_geometry(geo, style);
	}

	int Qt_geometry_layer::add_geometry(Geometry geo, const Style & style)
	{
		int id = get_next_geometry_id();
		m_geometries[id] = QPair<Geometry,Style>(geo,style);
		return id;
	}

	void Qt_geometry_layer::clear()
	{
		m_geometries.clear();
	}

	void Qt_geometry_layer::draw()
	{
		QPainter & drawer = widget->get_painter();

		drawer.save();

		auto iter = m_geometries.begin();
		while (iter != m_geometries.end())
		{
			auto & geo = iter.value();
			if (geo.first.type == Geometry::Point)
			{
				drawer.setPen(geo.second.border_color);
				drawer.setBrush(geo.second.fill_color);
				drawer.drawEllipse(convert(geo.first.pt),geo.second.size,geo.second.size);
			}
			else if (geo.first.type == Geometry::Polyline)
			{

			}
			else if (geo.first.type == Geometry::Ellipse)
			{
				drawer.setPen(QPen(geo.second.border_color,geo.second.border_width,
							geo.second.border_type));
				drawer.setBrush(geo.second.fill_color);
				drawer.drawEllipse(convert(geo.first.pt),convert(geo.first.width),convert(geo.first.length));
			}
			else if (geo.first.type == Geometry::Rect)
			{

			}
			else if (geo.first.type == Geometry::Polygon)
			{

			}
			else
			{

			}
			++iter;
		}

		drawer.restore();
	}

	QPointF Qt_geometry_layer::convert(const QPointF & pt)
	{
		return QPointF(widget->x_pixel(pt.x()),widget->y_pixel(pt.y()));
	}

	double Qt_geometry_layer::convert(double dis)
	{
		return widget->x_pixel_dist(dis);
	}

}