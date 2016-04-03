#include "mywidget.h"
#include "../layerwidget/Qt_widget_zoomrect.h"
#include "../layerwidget/Qt_gird_layer.h"
#include "../layerwidget/Qt_widget_handtool.h"


mywidget::mywidget(QWidget * parent)
	:Qt_widget(parent)
{
//	attach(new Qt_widget_zoomrect);
	attach(new Qt_gird_layer);
	attach(new Qt_widget_handtool);
}


mywidget::~mywidget(void)
{
}
