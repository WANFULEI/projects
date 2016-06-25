#ifndef FRAMEGUI_H
#define FRAMEGUI_H

#include "framegui_global.h"
#include <QWidget>
#include <QResizeEvent>
#include "../../template/singleton.h"
#include <QMainWindow>
#include <QStatusBar>

namespace framegui
{
	class FRAMEGUI_EXPORT framegui : public instance<framegui>
	{
	public:
		framegui(){
			m_main_window = 0;
		}

		template<typename T>
		T * get_type_object(const QString & id)
		{
			for (int i=0;i<m_objects.size();++i)
			{
				if (m_objects[i] == 0)
				{
					m_objects.erase(m_objects.begin() + i);
					--i;
					continue;
				}
				if (m_objects[i]->objectName() == id && dynamic_cast<T *>(m_objects[i]) )
				{
					return dynamic_cast<T *>(m_objects[i]);
				}
			}
			return 0;
		}

		void add_object(QObject * object){
			if (object == 0 || m_objects.contains(object))
			{
				return;
			}
			m_objects << object;
		}
		void remove_object(QObject * object){
			m_objects.removeOne(object);
		}

		QMainWindow * get_main_window() const { return m_main_window; }
		void set_main_window(QMainWindow * wgt) { m_main_window = wgt; }
		QStatusBar * status_bar() const {
			if(m_main_window)
				return m_main_window->statusBar();
			return 0;
		}

	private:
		QObjectList m_objects;
		QMainWindow * m_main_window;
	};

	class FRAMEGUI_EXPORT center_widget : public QWidget
	{
		typedef QWidget parent_class;
	public:
		center_widget(QWidget * parent = 0) 
			:QWidget(parent){ 
				m_widget = 0;
		}
		~center_widget(){}
		void set_widget(QWidget * widget){
			if (m_widget)
			{
				delete m_widget;
			}
			m_widget = widget;
			if (m_widget)
			{
				m_widget->setParent(this);
				m_widget->move(0,0);
				m_widget->resize(width(),height());
			}
		}
		QWidget * get_widget() const { return m_widget; }

	protected:
		void resizeEvent(QResizeEvent * event){
			if (m_widget)
			{
				m_widget->resize(event->size());
			}
			parent_class::resizeEvent(event);
		}

	protected:
		QWidget * m_widget;
	};
}



#endif // FRAMEGUI_H
