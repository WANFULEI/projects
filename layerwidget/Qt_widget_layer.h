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
// Author(s)     : Laurent Rineau and Radu Ursu

#ifndef CGAL_QT_WIDGET_LAYER_H
#define CGAL_QT_WIDGET_LAYER_H

/*#include <CGAL/IO/Qt_widget.h>*/
#include <QtCore/qobject.h>
#include <QtGui/qcursor.h>
#include <list>
#include <QtGui/QMouseEvent>
#include "layerwidget_global.h"
#include "../baseset2/baseset2.h"


namespace layerwidget {

class Qt_widget;
class LAYERWIDGET_EXPORT Qt_widget_layer : public QObject , public baseset::share_obj{
  Q_OBJECT
public:
  Qt_widget_layer(QObject* parent = 0, const char* name = 0) 
    : QObject(parent), does_eat_events(false), active(false){
		setObjectName(name);
  };

  // Event handlers
  virtual void mousePressEvent(QMouseEvent *) {} ;
  virtual void mouseReleaseEvent(QMouseEvent *) {};
  virtual void wheelEvent(QWheelEvent *) {};
  virtual void mouseDoubleClickEvent(QMouseEvent *) {};
  virtual void mouseMoveEvent(QMouseEvent *) {};
  virtual void keyPressEvent(QKeyEvent *) {};
  virtual void keyReleaseEvent(QKeyEvent *) {};
  virtual void enterEvent(QEvent *) {};
  virtual void leaveEvent(QEvent *) {};
  virtual bool event(QEvent *e) {QObject::event(e); return true;};

  bool    is_active(){return active;};	//return true if this layer is active
  bool    does_eat_events;
  	bool Visible() const { return m_bVisible; }
	void Visible(bool val) { m_bVisible = val; }
public slots:
  virtual void draw(){};
  void    stateChanged(int);
  void    toggle(bool);
  bool    activate(); //activate and return true if it was not active
  bool    deactivate();//deactivate and return true if it was active
signals:
  void    activated(Qt_widget_layer*);
  void    deactivated(Qt_widget_layer*);
private:
  
  bool    active;	//true if this layers is active
  friend class Qt_widget;
protected:
  Qt_widget  *widget;//the pointer to the widget
  virtual void activating(){};
  virtual void deactivating(){};
  virtual void    attach(Qt_widget *w);//attach Qt_widget to the tool

private:
	bool m_bVisible;
};

} // namespace CGAL end

#endif // CGAL_QT_WIDGET_LAYER_H
