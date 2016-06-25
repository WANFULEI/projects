#ifndef LAYER_WIDGET_H
#define LAYER_WIDGET_H

#include "layer_widget_global.h"
#include "..\coordinate_widget\coordinate_widget.h"
#include "layer.h"
#include "..\..\template\manager.h"

namespace layer_wgt{

class LAYER_WIDGET_EXPORT layer_widget : public coordinate_wgt::coordinate_widget
{
	friend class swapbuffer_worker;
	Q_OBJECT
public:
	layer_widget();
	~layer_widget();

	void attach(layer *l);
	void set_active_layer(layer *l);


protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	//bool event(QEvent *e);
	void initializeGL(); 
	void paintGL();      
	void resizeGL( int width, int height ); 

private:
	int get_layer_index(layer *l);

private:
	int active_layer;
	share_list_vector_manager<layer> layers;
	volatile bool swapbuffer_down;
};

}
#endif // LAYER_WIDGET_H
