#include "Qt_gird_layer.h"
#include "Qt_widget.h"


namespace layerwidget
{
	Qt_gird_layer::Qt_gird_layer(void)
	{
	}


	Qt_gird_layer::~Qt_gird_layer(void)
	{
	}

	void Qt_gird_layer::draw()
	{
		widget->get_painter().drawLine(widget->x_pixel(widget->x_min()),0,
				widget->x_pixel(widget->x_max()),0);
		widget->get_painter().drawLine(0,widget->y_pixel(widget->y_min()),
				0,widget->y_pixel(widget->y_max()));
	}

}
