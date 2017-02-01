#pragma once
#include "qt_widget_layer.h"
#include "layerwidget_global.h"

namespace layerwidget
{

class LAYERWIDGET_EXPORT Qt_tools_layer :
	public Qt_widget_layer
{
public:
	Qt_tools_layer(void);
	~Qt_tools_layer(void);

	virtual void wheelEvent(QWheelEvent *);

};

class LAYERWIDGET_EXPORT Qt_pan_layer :
	public Qt_tools_layer
{
public:

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

	QPoint m_pt_down;
};

class LAYERWIDGET_EXPORT Qt_select_layer :
	public Qt_tools_layer
{
	Q_OBJECT
public:
	Qt_select_layer(void){
		isdrawed = false;
	}

	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void attach(Qt_widget *w);
	bool isdrawed;

private slots:
	void redraw_on_back();
};

}