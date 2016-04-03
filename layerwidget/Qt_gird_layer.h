#pragma once
#include "qt_widget_layer.h"

namespace layerwidget
{
	class Qt_gird_layer :
		public Qt_widget_layer
	{
	public:
		Qt_gird_layer(void);
		~Qt_gird_layer(void);

		void draw();
	};
}

