#pragma once
#include "layer.h"
#include "layer_widget_global.h"

class QWheelEvent;
class QMouseEvent;
namespace layer_wgt
{

 class LAYER_WIDGET_EXPORT wheel_layer : public layer
 {
 public:
 	wheel_layer(void);
 	~wheel_layer(void);
 
 	virtual void wheelEvent(QWheelEvent *);
 };

class LAYER_WIDGET_EXPORT pan_layer :
	public wheel_layer
{
public:

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

	QPoint m_pt_down;
};

// class LAYER_WIDGET_EXPORT select_layer :
// 	public wheel_layer
// {
// 	Q_OBJECT
// public:
// 	select_layer(void){
// 		isdrawed = false;
// 	}
// 
// 	virtual void mouseMoveEvent(QMouseEvent *);
// 	virtual void attach(Qt_widget *w);
// 	bool isdrawed;
// 
// private slots:
// 	void redraw_on_back();
// };

}