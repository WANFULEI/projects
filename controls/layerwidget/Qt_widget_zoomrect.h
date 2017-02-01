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

#ifndef CGAL_QT_WIDGET_ZOOMRECT_H
#define CGAL_QT_WIDGET_ZOOMRECT_H

// #include <CGAL/IO/Qt_widget.h>
// #include <CGAL/IO/Qt_widget_layer.h>
#include <QtCore/qrect.h>
#include <QtGui/qcursor.h>
#include <QtGui/QMouseEvent>
#include "qt_widget_layer.h"
#include "layerwidget_global.h"

#ifndef CGAL_QT_WIDGET_ZOOMRECT_BUTTON
#define CGAL_QT_WIDGET_ZOOMRECT_BUTTON Qt::LeftButton
#endif


namespace layerwidget {

class Qt_widget_zoomrect : public Qt_widget_layer
{
public:
  int   first_x, first_y, x2, y2;
  bool  widgetrepainted;
  bool  on_first;
  Qt::FocusPolicy	oldpolicy;
  Qt_widget_zoomrect(QObject* parent = 0, const char* name = 0)
    : Qt_widget_layer(parent, name), widgetrepainted(1),
      on_first(0) {};

private:
  QCursor oldcursor;

  bool is_pure(Qt::KeyboardModifiers s){
    if((s & Qt::ControlModifier) ||
       (s & Qt::ShiftModifier) ||
       (s & Qt::AltModifier))
      return 0;
    else
      return 1;
  }

  void draw(){
    widgetrepainted = 1;
  };
  void mousePressEvent(QMouseEvent *e)
  {
    if(e->button() == CGAL_QT_WIDGET_ZOOMRECT_BUTTON
       && is_pure(e->modifiers()))
    {
      if (!on_first)
      {
        first_x = e->x();
        first_y = e->y();
        on_first = true;
      }
    }
  };

  void mouseReleaseEvent(QMouseEvent *e)
  {
    if(e->button() == CGAL_QT_WIDGET_ZOOMRECT_BUTTON
       && is_pure(e->modifiers()))
    {
      if((e->x() != first_x) && (e->y() != first_y)) {
        double x, y, xfirst2, yfirst2;
        widget->x_real(e->x(), x);
        widget->y_real(e->y(), y);
        widget->x_real(first_x, xfirst2);
        widget->y_real(first_y, yfirst2);
  			
        double	xmin, xmax, ymin, ymax;
        if(x < xfirst2) {xmin = x; xmax = xfirst2;}
        else {xmin = xfirst2; xmax = x;};
        if(y < yfirst2) {ymin = y; ymax = yfirst2;}
        else {ymin = yfirst2; ymax = y;};

        widget->set_window(xmin, xmax, ymin, ymax);        
        on_first = 0;
      }
    }
  }
  void mouseMoveEvent(QMouseEvent *e)
  {
    if(on_first)
    {
      int x = e->x();
      int y = e->y();
      //*widget << noFill;
      //QPainter::CompositionMode old = widget->rasterOp();	//save the initial raster mode
      //QColor old_color=widget->color();
      widget->get_painter().save();
	   widget->lock();

	  widget->get_painter().setCompositionMode(QPainter::RasterOp_SourceXorDestination);
     
      widget->setColor(QColor(255,0,255,255));
      if(!widgetrepainted)
        widget->get_painter().drawRect(first_x, first_y, 
                                       x2 - first_x, y2 - first_y);
	  
      widget->get_painter().drawRect(first_x, first_y, x - first_x,
                                     y - first_y);

	 // widget->setRasterOp(old);
      widget->unlock();
     // widget->setColor(old_color);
	  widget->get_painter().restore();
      

      //save the last coordinates to redraw the screen
      x2 = x;
      y2 = y;
      widgetrepainted = false;
    }
  };

  void keyPressEvent(QKeyEvent *e)
  {
    switch ( e->key() ) {
	case Qt::Key_Escape:			// key_escape
         if (on_first)
         {
           widget->lock();
           *widget << noFill;
           QPainter::CompositionMode old = widget->rasterOp();	//save the initial raster mode
           QColor old_color=widget->color();
           widget->setRasterOp(QPainter::RasterOp_SourceXorDestination);
          // *widget << CGAL::GREEN;
		   widget->setColor(Qt::green);
           if(!widgetrepainted)
             widget->get_painter().drawRect(first_x, first_y, 
                                       x2 - first_x, y2 - first_y);
           widget->setColor(old_color);
           widget->setRasterOp(old);
           widgetrepainted = true;

           widget->unlock();
	   on_first = false;
         }
         break;
    }//endswitch
  }

  void leaveEvent(QEvent *)
  {
    if (on_first)
    {
      widget->lock();
      *widget << noFill;
      QPainter::CompositionMode old = widget->rasterOp();	//save the initial raster mode
      QColor old_color=widget->color();
      widget->setRasterOp(QPainter::RasterOp_SourceXorDestination);
    //  *widget << CGAL::GREEN;
	  widget->setColor(Qt::green);
      if(!widgetrepainted)
        widget->get_painter().drawRect(first_x, first_y, 
                                       x2 - first_x, y2 - first_y);
      widget->setColor(old_color);
      widget->setRasterOp(old);
      widgetrepainted = true;

      widget->unlock();
    }
  }

  void activating()
  {
    oldpolicy = widget->focusPolicy();
    widget->setFocusPolicy(Qt::StrongFocus);
    oldcursor = widget->cursor();
    widget->setCursor(Qt::CrossCursor);
    widgetrepainted = true;
  };

  void deactivating()
  {
    widget->setCursor(oldcursor);
    widget->setFocusPolicy(oldpolicy);
    on_first = false;
    widget->redraw();
  };
};//end class 

} // namespace CGAL

#endif // CGAL_QT_WIDGET_ZOOMRECT_H
