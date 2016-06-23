#pragma once
#include "..\..\base\baseset2\baseset2.h"
#include "layer_widget_global.h"

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QEvent;
namespace layer_wgt{

class LAYER_WIDGET_EXPORT layer : public baseset::share_obj
{
	friend class layer_widget;
public:
	layer(void);
	virtual ~layer(void);
	void set_min_lod(double lod) { min_lod = lod; }
	void set_max_lod(double lod) { max_lod = lod; }
	double get_min_lod() const { return min_lod; }
	double get_max_lod() const { return max_lod; }

protected:
	virtual void mousePressEvent(QMouseEvent *) {};
	virtual void mouseReleaseEvent(QMouseEvent *) {};
	virtual void wheelEvent(QWheelEvent *) {};
	virtual void mouseDoubleClickEvent(QMouseEvent *) {};
	virtual void mouseMoveEvent(QMouseEvent *) {};
	virtual void keyPressEvent(QKeyEvent *) {};
	virtual void keyReleaseEvent(QKeyEvent *) {};
	virtual void enterEvent(QEvent *) {};
	virtual void leaveEvent(QEvent *) {};
	virtual bool event(QEvent *e) { return false; };
	virtual void gl_draw() {};

private:
	double max_lod,min_lod;
protected:
	layer_widget * widget;
};

}

