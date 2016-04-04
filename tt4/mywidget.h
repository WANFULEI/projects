#pragma once
#include "..\layerwidget\qt_widget.h"

using namespace layerwidget;

class mywidget :
	public Qt_widget
{
public:
	mywidget(QWidget * parent);
	~mywidget(void);

	void Pan();
	void ZoomIn();

};

