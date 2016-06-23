#include "coordinate_widget.h"

namespace coordinate_wgt{

coordinate_widget::coordinate_widget(void)
{
	xmin_old = xmin = -1;
	xmax_old = xmax = 1;
	ymin_old = ymin = -1;
	ymax_old = ymax = 1;
	constranges=false;
	set_scales();
	emit(rangesChanged());
}

void coordinate_widget::set_window(const double x_min, const double x_max, const double y_min, const double y_max, bool const_ranges /*= false*/)
{
	xmin_old = xmin = x_min;
	xmax_old = xmax = x_max;
	ymin_old = ymin = y_min;
	ymax_old = ymax = y_max;
	constranges = const_ranges;
	set_scales();
	rebuild_coor();
	redraw();
	emit(rangesChanged());
}

void coordinate_widget::move_center(const double distx, const double disty)
{
	xmin += distx; xmin_old += distx;
	xmax += distx; xmax_old += distx;
	ymin += disty; ymin_old += disty;
	ymax += disty; ymax_old += disty;
	rebuild_coor();
	redraw();
	emit(rangesChanged());
}

void coordinate_widget::set_center(const double x, const double y)
{
	if (set_scales_to_be_done) return;

	if(xscal<1) {
		xmin = x - (int)(width()/xscal)/2;
		xmax = x + (int)(width()/xscal)/2;
		ymin = y - (int)(height()/yscal)/2;
		ymax = y + (int)(height()/yscal)/2;
	} else {
		xmin = x - (width()/xscal)/2;
		xmax = x + (width()/xscal)/2;
		ymin = y - (height()/yscal)/2;
		ymax = y + (height()/yscal)/2;
	}
	xmin_old = xmin;
	xmax_old = xmax;
	ymin_old = ymin;
	ymax_old = ymax; 
	rebuild_coor();
	redraw();
	emit(rangesChanged());
}

void coordinate_widget::resizeEvent(QResizeEvent *event)
{
	xmin = xmin_old;
	xmax = xmax_old;
	ymin = ymin_old;
	ymax = ymax_old;
	set_scales();

	QGLWidget::resizeEvent(event);
}

void coordinate_widget::rebuild_coor()
{
	glViewport(0, 0, (GLint)width(), (GLint)height()); 
	glMatrixMode( GL_PROJECTION );  
	glLoadIdentity();
	glOrtho( xmin, xmax, ymin, ymax, -1, 1 );
// 	glMatrixMode(GL_MODELVIEW);
// 	glLoadIdentity();
}

void coordinate_widget::set_scales()
{
	if( ! isVisible() )
	{
		set_scales_to_be_done = true;
		return;
	};
	set_scales_to_be_done = false;

	if(!constranges)
	{
		xscal = yscal = qMin( width() / (xmax - xmin),
			height() / (ymax - ymin) );
		double xcenter = xmin + (xmax - xmin) / 2;
		double ycenter = ymin + (ymax - ymin) / 2;

		if(xscal<1) {
			// if xscal < 1, width()/xscal > width(). then we can round it 
			// with loosing precision.
			xmin = xcenter - (int)(width()/xscal)/2;
			xmax = xcenter + (int)(width()/xscal)/2;
			ymin = ycenter - (int)(height()/yscal)/2;
			ymax = ycenter + (int)(height()/yscal)/2;
		} else {
			xmin = xcenter - (width()/xscal)/2;
			xmax = xcenter + (width()/xscal)/2;
			ymin = ycenter - (height()/yscal)/2;
			ymax = ycenter + (height()/yscal)/2;
		}
	}
	else
	{
		xscal=width()/(xmax-xmin);
		yscal=height()/(ymax-ymin);
	}
}

}