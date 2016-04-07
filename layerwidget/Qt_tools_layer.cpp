#include "Qt_tools_layer.h"
#include "Qt_widget.h"

namespace layerwidget
{


Qt_tools_layer::Qt_tools_layer(void)
{
}


Qt_tools_layer::~Qt_tools_layer(void)
{
}


void layerwidget::Qt_tools_layer::wheelEvent(QWheelEvent * event)
{
	if (event->delta() > 0)
	{
		widget->zoom(1.25);
	}
	else
	{
		widget->zoom(1/1.25);
	}
}

void Qt_pan_layer::mousePressEvent(QMouseEvent * event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_pt_down = event->pos();
		widget->setCursor(Qt::ClosedHandCursor);
	}
}

void Qt_pan_layer::mouseMoveEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		double offset_x = widget->x_real_dist(event->pos().x() - m_pt_down.x());
		double offset_y = widget->y_real_dist(event->pos().y() - m_pt_down.y());
		widget->move_center(-offset_x,offset_y);
		m_pt_down = event->pos();
	}
}

void Qt_pan_layer::mouseReleaseEvent(QMouseEvent *)
{

}

void Qt_select_layer::mouseMoveEvent(QMouseEvent * event)
{
	widget->get_painter().save();
	widget->lock();

	static QString sText;
	static QPoint pt;

	widget->get_painter().setPen(QColor(255,0,255));
	widget->get_painter().setCompositionMode(QPainter::RasterOp_SourceXorDestination);

	QString s = QObject::tr("%1,%2").arg(widget->x_real(event->pos().x())).arg(widget->y_real(event->pos().y()));
	if (isdrawed)
	{
		widget->get_painter().drawText(pt,sText);
	}
	widget->get_painter().drawText(event->pos(),s);

	sText = s;
	pt = event->pos();
	isdrawed = true;

	widget->unlock();
	widget->get_painter().restore();
}

void Qt_select_layer::attach(Qt_widget *w)
{
	__super::attach(w);
	connect(w,SIGNAL(redraw_on_back()),this,SLOT(redraw_on_back()));
}

void Qt_select_layer::redraw_on_back()
{
	isdrawed = false;
}

}