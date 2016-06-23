#include "gird_layer.h"
#include "layer_widget.h"


namespace layer_widget
{
	gird_layer::gird_layer(void)
	{
		m_gird_line_color = Qt::gray;
// 		m_last_start_x = widget->x_min();
// 		m_last_start_y = widget->y_min();
	}


	gird_layer::~gird_layer(void)
	{
	}

	void gird_layer::draw()
	{
		widget->get_painter().save();

		//draw ver gird
		
		int cx,cy;
		cx = widget->width();
		cy = widget->height();

		int x = 80;
		widget->get_painter().setPen(QPen(m_gird_line_color,1,Qt::DotLine));
		while (x < cx)
		{
			QLine line(x,0,x,cy-1);
			widget->get_painter().drawLine(line);
			x += 80;
		}
		x = 80;
		widget->get_painter().setPen(QPen(m_gird_line_color,1,Qt::SolidLine));
		while (x < cx)
		{
			QLine line(x,0,x,cy-1);
			widget->get_painter().drawText(line.p2(),QString::number(widget->x_real(x)));
			x += 80;
		}

		//draw hor gird
		int y = cy - 80;
		widget->get_painter().setPen(QPen(m_gird_line_color,1,Qt::DotLine));
		while (y > 0)
		{
			QLine line(0,y,cx-1,y);			
			widget->get_painter().drawLine(line);
			y -= 80;
		}
		y = cy - 80;
		widget->get_painter().setPen(QPen(m_gird_line_color,1,Qt::SolidLine));
		while (y > 0)
		{
			QLine line(0,y,cx-1,y);			
			widget->get_painter().drawText(line.p1(),QString::number(widget->y_real(y)));
			y -= 80;
		}

		widget->get_painter().restore();
	}

	void Qt_image_layer::draw()
	{
		widget->get_painter().drawPixmap(widget->rect(),m_image);
	}

}
