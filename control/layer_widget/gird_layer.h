#pragma once
#include <QColor>
#include "layer.h"
#include "layer_widget_global.h"
#include <QPixmap>

namespace layer_widget
{
	class LAYER_WIDGET_EXPORT gird_layer :
		public layer
	{
	public:
		gird_layer(void);
		~gird_layer(void);

		void draw();

		QColor m_gird_line_color;
		//double m_last_start_x,m_last_start_y;
	};

	class LAYER_WIDGET_EXPORT Qt_image_layer :
		public layer
	{
	public:

		void set_image(const QPixmap & image) { m_image = image; }

		void draw();

		QPixmap m_image;
	};
}

