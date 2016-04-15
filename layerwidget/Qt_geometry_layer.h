#pragma once

#include "../../layerwidget/qt_widget_layer.h"
#include "layerwidget_global.h"
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include "../baseset2/baseset2.h"
#include <QtGui/QFont>

using namespace layerwidget;

namespace layerwidget
{
	struct Geometry
	{
		QPointF pt;
		QList<QPointF> pts;
		double radius;
		double length,width;
		double start_angle,end_angle;
		enum Type{ None , Point , Picture , Polyline , Ellipse , Rect , Polygon } type;
	};

	struct Style 
	{
		QPixmap pixmap;
		int size;
		QString info;
		QColor border_color,fill_color;
		int border_width;
		Qt::PenStyle border_type;
		QFont font;
		Style(){
			size = 6;
			border_color = Qt::green;
			fill_color = Qt::transparent;
			border_width = 1;
			border_type = Qt::SolidLine;
			font = QFont("ËÎÌו",9);
		}
	};

	struct Feature : public baseset::share_obj
	{
		Feature(const Geometry & geometry = Geometry(),const Style & style = Style())
			:style(style)
		{
			geo = geometry;
		}
		Geometry geo;
		Style style;
	};

	class LAYERWIDGET_EXPORT Qt_geometry_layer :
		public Qt_widget_layer
	{
	public:
		Qt_geometry_layer(void);
		~Qt_geometry_layer(void);

		void add_points(const QList<QPointF> & pts, const QColor & color,int size = 3);

		int add_ellipse(const QPointF & pt, double length, double width, const Style & style = Style());
		int add_polygon(const QList<QPointF> & pts,const Style & style = Style());
		int add_feature(Geometry geo, const Style & style);
		void clear();

		baseset::share_map_manager<int,Feature> & get_geometries() { return m_features; }
		baseset::share_ptr<Feature> get_feature(int id) const;
		int get_feature_id(const baseset::share_ptr<Feature> & feature) const;

	protected:
		void draw();
	protected:
		int get_next_geometry_id();
		QPointF convert(const QPointF & pt);
		QVector<QPointF> convert(const QList<QPointF> & pts);
		double convert(double dis);
	protected:
		baseset::share_map_manager<int,Feature> m_features;
		static int m_next_feature_id;
	};

	class LAYERWIDGET_EXPORT Qt_geometry_layer_manager :
		public baseset::share_list_vector_manager<Qt_geometry_layer> , public baseset::share_obj
	{
	public:
		baseset::share_ptr<Feature> get_feature(int id);
		int get_feature_id(const baseset::share_ptr<Feature> & feature);
	};
}



