#ifndef COORDINATE_WIDGET_H
#define COORDINATE_WIDGET_H

#include <QGLWidget>
#include "coordinate_widget_global.h"
#include <GL\glu.h>

namespace coordinate_wgt{

class COORDINATE_WIDGET_EXPORT coordinate_widget : public QGLWidget
{
	Q_OBJECT
public:
	coordinate_widget(void);
	~coordinate_widget(void){}

	// initialization of coordinates system
	void set_window(const double x_min,
		const double x_max, 
		const double y_min, 
		const double y_max,
		bool const_ranges = false);
	void zoom(double ratio){
		zoom(ratio,
			xmin + (xmax - xmin) / 2 ,
			ymin + (ymax - ymin) / 2 );
	}
	void zoom(double ratio, double xc, double yc){
		xscal = xscal*ratio; yscal = yscal*ratio;
		set_center(xc, yc);
	}
	void set_x_scale(const double xscale){ xscal = xscale; }
	void set_y_scale(const double yscale){ yscal = yscale; }
	void move_center(const double distx, const double disty);
	void set_center(const double x, const double y);

	// coordinates system
	// ~~~~~~~~~~~~~~~~~~
	// real world coordinates
	double x_real(int x) const{
		if(xscal<1)
			return(xmin+(int)(x/xscal));
		else
			return (xmin+x/xscal);
	}
	double y_real(int y) const{
		if(yscal<1)
			return(ymax-(int)(y/yscal));
		else
			return (ymax-y/yscal);
	}
	// pixel coordinates
	int x_pixel(double x) const {
		return( static_cast<int>((x-xmin)*xscal) );
	}
	int y_pixel(double y) const{
		return( - static_cast<int>((y-ymax)*yscal) );
	}
	double x_real_dist(double d) const{
		return(d/xscal);
	}
	double y_real_dist(double d) const{
		return(d/yscal);
	}
	int x_pixel_dist(double d) const{
		if (d>0)
			return( static_cast<int>(d*xscal) );
		else
			return( static_cast<int>(d*xscal) );
	}
	int y_pixel_dist(double d) const{
		if (d>0)
			return( static_cast<int>(d*yscal) );
		else
			return( static_cast<int>(d*yscal) );
	}

	inline double x_min() const { return xmin; };
	inline double y_min() const { return ymin; };
	inline double x_max() const { return xmax; };
	inline double y_max() const { return ymax; };
	inline double x_scal() { return xscal; }
	inline double y_scal() { return yscal; }

protected:
	void resizeEvent(QResizeEvent *event);
	void rebuild_coor();
	


private:
	void	 set_scales();
	void redraw() { glDraw(); }

signals:
	void rangesChanged();

private:
	double    xmin, xmax, ymin, ymax; // real dimensions
	double    xmin_old, xmax_old, ymin_old, ymax_old;
	//backup ranges for resize
	double    xscal, yscal; // scales int/double
	bool      constranges; // tell if the ranges should be const
	bool    set_scales_to_be_done;
};

}
#endif // COORDINATE_WIDGET_H
