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
		virtual bool load_from_xml(TiXmlElement * pXmlNode);
		virtual bool initialize();
		virtual void action_triggered(QAction * action);

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
}



#endif // DEMO_CORE_H
