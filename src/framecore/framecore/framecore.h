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
		// Qualifier: 用户可以在application.cfg配置文件中自己组件节点下，添加一些配置信息，重写
		//			 此函数读取配置信息，之后调用
		// Parameter: TiXmlElement * pXmlNode
		//************************************
		virtual bool load_from_xml(TiXmlElement * pXmlNode);
		//************************************
		// Method:    initialize
		// FullName:  demo_core::component::initialize
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier: 组件初始化函数
		//************************************
		virtual bool initialize();
		//************************************
		// Method:    action_triggered
		// FullName:  demo_core::component::action_triggered
		// Access:    virtual public 
		// Returns:   void
		// Qualifier: 如果在menu.cfg菜单配置文件中，组件处理此菜单，菜单触发时回调用此函数
		// Parameter: QAction * action
		//************************************
		virtual void action_triggered(QAction * action);
		//************************************
		// Method:    on_create_control
		// FullName:  demo_core::component::on_create_control
		// Access:    virtual public 
		// Returns:   void
		// Qualifier: 框架通过toolbar.cfg配置文件创建工具条时，创建控件完成后，如果组件处理此控件，
		//			 会调用此函数，组件可根据id和控件指针做一些初始化操作
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
