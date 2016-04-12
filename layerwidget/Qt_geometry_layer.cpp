
#include "Qt_geometry_layer.h"
#include "..\..\layerwidget\Qt_widget.h"

int Qt_geometry_layer::m_next_geometry_id = 1;
namespace layerwidget
{
	Qt_geometry_layer::Qt_geometry_layer(void)
	{
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

	int Qt_geometry_layer::add_polygon(const QList<QPointF> & pts,const Style & style /*= Style()*/)
	{
		Geometry geo;
		geo.pts = pts;
		geo.type = Geometry::Polygon;

		return add_geometry(geo, style);
	}

	int Qt_geometry_layer::add_geometry(Geometry geo, const Style & style)
	{
		int id = get_next_geometry_id();
		m_geometries[id] = new Feature(geo,style);
		return id;
	}

	Feature * Qt_geometry_layer::get_feature(int id) const
	{
		auto iter = m_geometries.find(id);
		if (iter == m_geometries.end())
		{
			return 0;
		}
		return iter.value();
	}

	int Qt_geometry_layer::get_feature_id(Feature * feature) const
	{
		if (feature == 0)
		{
			return 0;
		}
		auto iter = m_geometries.begin();
		while (iter != m_geometries.end())
		{
			if (feature == iter.value())
			{
				return iter.key();
			}
			++iter;
		}
		return 0;
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
			Feature * feature = iter.value();

			drawer.setPen(QPen(feature->style.border_color,feature->style.border_width,
				feature->style.border_type));
			drawer.setBrush(feature->style.fill_color);
			
			if (feature->geo.type == Geometry::Point)
			{
				drawer.drawEllipse(convert(feature->geo.pt),feature->style.size,feature->style.size);
			}
			else if (feature->geo.type == Geometry::Polyline)
			{

			}
			else if (feature->geo.type == Geometry::Ellipse)
			{

				drawer.drawEllipse(convert(feature->geo.pt),convert(feature->geo.width),convert(feature->geo.length));
			}
			else if (feature->geo.type == Geometry::Rect)
			{
				
			}
			else if (feature->geo.type == Geometry::Polygon)
			{
				drawer.drawPolygon(convert(feature->geo.pts));
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

	QVector<QPointF> Qt_geometry_layer::convert(const QList<QPointF> & pts)
	{
		QVector<QPointF> res;
		for (int i=0;i<pts.size();++i)
		{
			res << convert(pts[i]);
		}
		return res;
	}

	Qt_geometry_layer_manager * Qt_geometry_layer_manager::GetInstance()
	{
		static Qt_geometry_layer_manager manager;
		return &manager;
	}

	Feature * Qt_geometry_layer_manager::get_feature(int id)
	{
		if (id <= 0)
		{
			return 0;
		}
		for (int i=0;i<size();++i)
		{
			Qt_geometry_layer * layer = get_entry(i);
			if (layer == 0)
			{
				continue;
			}
			Feature * feature = layer->get_feature(id);
			if (feature)
			{
				return feature;
			}
		}
		return 0;
	}

	int Qt_geometry_layer_manager::get_feature_id(Feature * feature)
	{
		if (feature == 0)
		{
			return 0;
		}
		for (int i=0;i<size();++i)
		{
			Qt_geometry_layer * layer = get_entry(i);
			if (layer == 0)
			{
				continue;
			}
			int id = layer->get_feature_id(feature);
			if (id)
			{
				return id;
			}
		}
		return 0;
	}

}