#pragma once
#include "QWidget"

#include "../../control/layerwidget/Qt_default_widget.h"

class simple_map :
	public layerwidget::Qt_default_widget
{
public:
	simple_map(QWidget * parent = 0);
	~simple_map(void);
};

