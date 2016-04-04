#include "mywidget.h"
#include "../layerwidget/Qt_widget_zoomrect.h"
#include "../layerwidget/Qt_gird_layer.h"
#include "../layerwidget/Qt_widget_handtool.h"


mywidget::mywidget(QWidget * parent)
	:Qt_widget(parent)
{
	attach(new Qt_widget_zoomrect);
	attach(new Qt_gird_layer);
	attach(new Qt_widget_handtool);
	set_window(-180,180,-90,90,true);
	Pan();
}


mywidget::~mywidget(void)
{
}

void mywidget::Pan()
{
	std::list<Qt_widget_layer *> layers = get_layers();
	auto iter = layers.begin();
	while(iter != layers.end())
	{
		if (dynamic_cast<Qt_widget_handtool *>(*iter))
		{
			(*iter)->activate();
		}
		else
		{
			(*iter)->deactivate();
		}
		++iter;
	}
}

void mywidget::ZoomIn()
{
	std::list<Qt_widget_layer *> layers = get_layers();
	auto iter = layers.begin();
	while(iter != layers.end())
	{
		if (dynamic_cast<Qt_widget_zoomrect *>(*iter))
		{
			(*iter)->activate();
		}
		else
		{
			(*iter)->deactivate();
		}
		++iter;
	}
}
