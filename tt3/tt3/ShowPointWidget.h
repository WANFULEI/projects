#pragma once

#include <QtWidgets/QWidget>
#include "../../layerwidget/Qt_widget.h"
#include "..\..\layerwidget\Qt_geometry_layer.h"

using namespace layerwidget;

namespace tt3
{
	class ShowPointWidget :
		public Qt_widget
	{
	public:
		ShowPointWidget(QWidget * parent = 0);
		~ShowPointWidget(void);

		Qt_geometry_layer * Show_point_layer() const { return m_show_point_layer; }
		void Show_point_layer(Qt_geometry_layer * val) { m_show_point_layer = val; }

		Qt_geometry_layer * m_show_point_layer;
	};
}

