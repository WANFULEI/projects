#ifndef DEMO_CORE_H
#define DEMO_CORE_H

#include "demo_core_global.h"
#include <QtCore/QObject>
#include "../baseset2/baseset2.h"
#include "../tinyxml/tinyxml.h"
#include <QtGui/QAction>

namespace demo_core
{
	class DEMO_CORE_EXPORT object : public baseset::share_obj
	{
	public:
		QString get_name() const { return m_sName; }
		void set_name(const QString & val) { m_sName = val; }
		virtual bool load_from_xml(TiXmlElement * pXmlNode);

	protected:
		QString m_sName;
	};

	class DEMO_CORE_EXPORT component : public object
	{
	public:
		QString get_class_name() const { return m_sClassName; }
		void set_class_name(const QString & val) { m_sClassName = val; }
		//************************************
		// Method:    load_from_xml
		// FullName:  demo_core::component::load_from_xml
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier: �û�������application.cfg�����ļ����Լ�����ڵ��£����һЩ������Ϣ����д
		//			 �˺�����ȡ������Ϣ��֮�����
		// Parameter: TiXmlElement * pXmlNode
		//************************************
		virtual bool load_from_xml(TiXmlElement * pXmlNode);
		//************************************
		// Method:    initialize
		// FullName:  demo_core::component::initialize
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier: �����ʼ������
		//************************************
		virtual bool initialize();
		//************************************
		// Method:    action_triggered
		// FullName:  demo_core::component::action_triggered
		// Access:    virtual public 
		// Returns:   void
		// Qualifier: �����menu.cfg�˵������ļ��У��������˲˵����˵�����ʱ�ص��ô˺���
		// Parameter: QAction * action
		//************************************
		virtual void action_triggered(QAction * action);
		//************************************
		// Method:    on_create_control
		// FullName:  demo_core::component::on_create_control
		// Access:    virtual public 
		// Returns:   void
		// Qualifier: ���ͨ��toolbar.cfg�����ļ�����������ʱ�������ؼ���ɺ�����������˿ؼ���
		//			 ����ô˺���������ɸ���id�Ϳؼ�ָ����һЩ��ʼ������
		// Parameter: const QString & id
		// Parameter: QWidget * widget
		//************************************
		virtual void on_create_control(const QString & toolbar_id,const QString & id,QWidget * widget){}
		virtual QWidget * create_control(const QString & toolbar_id,const QString & control_id){ return 0; }

	protected:
		QString m_sClassName;
	};

	class DEMO_CORE_EXPORT application : public object
	{
	public:
		const baseset::share_list_vector_manager<component> & get_components() const { return m_components; }
		virtual bool load_from_xml(TiXmlElement * pXmlNode);
		baseset::share_ptr<demo_core::component> get_component(const QString & class_name) const;
	protected:
		baseset::share_list_vector_manager<component> m_components;
	};

	DEMO_CORE_EXPORT QString get_full_path(const QString & path);
}



#endif // DEMO_CORE_H
