#ifndef FRAMECORE_H
#define FRAMECORE_H

#include "framecore_global.h"
#include <QtCore/QObject>
#include "baseset/share_obj.h"
#include "baseset/manager.h"
#include "tinyxml.h"
#include <QtGui/QAction>

namespace framecore
{
	class FRAMECORE_EXPORT object : public share_obj
	{
	public:
		virtual ~object(){}
		QString get_name() const { return m_sName; }
		void set_name(const QString & val) { m_sName = val; }
		virtual bool load_from_xml(TiXmlElement * pXmlNode);

	protected:
		QString m_sName;
	};

	class FRAMECORE_EXPORT component : public object
	{
	public:
		virtual ~component(){}
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
		virtual void on_create_control(const QString & id,QWidget * widget){}
		virtual QWidget * create_control(const QString & control_id){ return 0; }

	protected:
		QString m_sClassName;
	};

	class FRAMECORE_EXPORT application : public object
	{
	public:
		virtual ~application(){}
		const share_list_vector_manager<component> & get_components() const { return m_components; }
		virtual bool load_from_xml(TiXmlElement * pXmlNode);
		share_ptr<framecore::component> get_component(const QString & class_name) const;
	protected:
		share_list_vector_manager<component> m_components;
	};


}



#endif // FRAMECORE_H
