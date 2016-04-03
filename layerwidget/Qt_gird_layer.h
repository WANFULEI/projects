#pragma once
#include <QtGui/QColor>
#include "qt_widget_layer.h"
#include "layerwidget_global.h"

namespace layerwidget
{
	class LAYERWIDGET_EXPORT Qt_gird_layer :
		public Qt_widget_layer
	{
	public:
		Qt_gird_layer(void);
		~Qt_gird_layer(void);

		void draw();

		QColor m_gird_line_color;
		//double m_last_start_x,m_last_start_y;
	};
}

