#include "stdafx.h"
#include "ShowPointWidget.h"



namespace tt3
{
	ShowPointWidget::ShowPointWidget(QWidget * parent)
		:Qt_widget(parent)
	{
		Show_point_layer(new Qt_geometry_layer());
		attach(Show_point_layer());
	}


	ShowPointWidget::~ShowPointWidget(void)
	{
	}
}
