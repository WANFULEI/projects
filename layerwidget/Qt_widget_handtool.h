// Copyright (c) 2002-2004  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 3 of the License,
// or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Radu Ursu

#ifndef CGAL_QT_WIDGET_HANDTOOL_H
#define CGAL_QT_WIDGET_HANDTOOL_H

#include <cstdio>
#include <QtCore/qrect.h>
#include <QtGui/qcursor.h>
#include "Qt_widget_layer.h"

#ifndef CGAL_QT_WIDGET_GET_POINT_BUTTON
#define CGAL_QT_WIDGET_GET_POINT_BUTTON Qt::LeftButton
#endif


namespace layerwidget {

class LAYERWIDGET_EXPORT Qt_widget_handtool : public Qt_widget_layer
{
public:
  Qt_widget_handtool(QObject* parent = 0, const char* name = 0)
    : Qt_widget_layer(parent,name), wasrepainted(1), on_first(0){};

private:
  QCursor oldcursor;

  void draw(){
    wasrepainted = 1;
  };

  void timerEvent( QTimerEvent *)
  {
    if(on_first)
      widget->setCursor(Qt::ClosedHandCursor);
    else
      widget->setCursor(Qt::OpenHandCursor);
  }

  bool is_pure(Qt::KeyboardModifiers s){
    if((s & Qt::ControlModifier) ||
       (s & Qt::ShiftModifier) ||
       (s & Qt::AltModifier))
      return 0;
    else
      return 1;
  }

  void mousePressEvent(QMouseEvent *e)
  {
    if(e->button() == CGAL_QT_WIDGET_GET_POINT_BUTTON 
       && is_pure(e->modifiers()))
    {
      widget->setCursor(Qt::ClosedHandCursor);
      if (!on_first){
	      first_x = e->x();
	      first_y = e->y();
	      on_first = 1;
      }	
    }
  };

  void mouseReleaseEvent(QMouseEvent *e)
  {
    if(e->button() == CGAL_QT_WIDGET_GET_POINT_BUTTON
       && is_pure(e->modifiers()))
    {
      widget->setCursor(Qt::OpenHandCursor);
      double x, y, xfirst2, yfirst2;
      widget->x_real(e->x(), x);
      widget->y_real(e->y(), y);
      widget->x_real(first_x, xfirst2);
      widget->y_real(first_y, yfirst2);
			
      double distx, disty;
      distx = xfirst2 - x;
      disty = yfirst2 - y;
      widget->move_center(distx, disty);
      on_first = 0;
    }
  }
  void mouseMoveEvent(QMouseEvent *e)
  {
    char tempc1[130], tempc2[40];
    double xcoord, ycoord;
    if(on_first)
    {
      int x = e->x();
      int y = e->y();
      //save the initial raster mode
      QPainter::CompositionMode old = widget->rasterOp();	
      widget->setRasterOp(QPainter::RasterOp_SourceXorDestination);
      widget->lock();
	  widget->setColor(Qt::gray);
      //  *widget << CGAL::GRAY;
      if(!wasrepainted) {
        widget->x_real(x2 - first_x, xcoord);
        widget->x_real(y2 - first_y, ycoord);
        std::sprintf(tempc1, " dx=%20.6f", xcoord);
        std::sprintf(tempc2, ", dy=%20.6f", ycoord);
        strcat(tempc1, tempc2);
        widget->get_painter().drawLine(first_x, first_y, x2, y2);
		widget->setColor(Qt::green);
        //*widget << CGAL::GREEN;
		widget->get_painter().drawText(x2, y2, tempc1);
        //widget->get_painter().drawText(x2, y2, tempc1, 49);
		widget->setColor(Qt::gray);
        //*widget << CGAL::GRAY;
      }
      widget->x_real(x - first_x, xcoord);
      widget->x_real(y - first_y, ycoord);
      std::sprintf(tempc1, " dx=%20.6f", xcoord);
      std::sprintf(tempc2, ", dy=%20.6f", ycoord);
      strcat(tempc1, tempc2);
      widget->get_painter().drawLine(first_x, first_y, x, y);
	  widget->setColor(Qt::green);
      //*widget << CGAL::GREEN;
	  widget->get_painter().drawText(x, y, tempc1);
      //widget->get_painter().drawText(x, y, tempc1, 49);
      widget->unlock();
      widget->setRasterOp(old);

      //save the last coordinates to redraw the screen
      x2 = x;
      y2 = y;
      wasrepainted = 0;
    }
  };

  void activating()
  {
    oldcursor = widget->cursor();
    widget->setCursor(Qt::OpenHandCursor);
    wasrepainted = 1;
	  startTimer( 100 );
  };

  void deactivating()
  {
    widget->setCursor(oldcursor);
    killTimer(1);
  };

  int   first_x, first_y;
  int   x2, y2;
  bool	wasrepainted;
  bool	on_first;
};//end class 

} // namespace CGAL

#endif // CGAL_QT_WIDGET_HANDTOOL_H
