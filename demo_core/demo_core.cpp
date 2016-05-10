#include "demo_core.h"
#include <QtCore/QLibrary>
#include <QtGui/QApplication>
#include <QFile>

typedef demo_core::component * (*FunctionPointer)(const QString & class_name);

bool demo_core::component::load_from_xml(TiXmlElement * pXmlNode)
{
	if (pXmlNode == 0)
	{
		return false;
	}
	if (pXmlNode->Value() != QString("component").toLower())
	{
		return false;
	}
	m_sClassName = pXmlNode->Attribute("classname");
	return object::load_from_xml(pXmlNode);
}

bool demo_core::component::initialize()
{
	return true;
}

void demo_core::component::action_triggered(QAction * action)
{

}

bool demo_core::object::load_from_xml(TiXmlElement * pXmlNode)
{
	if (pXmlNode == 0)
	{
		return false;
	}
	m_sName = pXmlNode->Attribute("name");
	return true;
}

bool demo_core::application::load_from_xml(TiXmlElement * pXmlNode)
{
	if (pXmlNode == 0)
	{
		return false;
	}
	if (pXmlNode->Value() != QString("application").toLower())
	{
		return false;
	}
	bool res = true;
	TiXmlElement * components = pXmlNode->FirstChildElement("components");
	while (components)
	{
		TiXmlElement * cmp = components->FirstChildElement("component");
		while (cmp)
		{
			QString sLibrary,sClassName;
			sLibrary = cmp->Attribute("library");
#if _DEBUG
			sLibrary += "d";
#endif
			sLibrary += ".dll";
			sClassName = cmp->Attribute("classname");
			QLibrary lib(qApp->applicationDirPath() + "/plugins/" + sLibrary);
			if (!lib.load())
			{
				res = false;
				cmp = cmp->NextSiblingElement("component");
				continue;
			}
			FunctionPointer create_component = (FunctionPointer)lib.resolve("create_component");
			if (create_component == 0)
			{
				res = false;
				cmp = cmp->NextSiblingElement("component");
				continue;
			}
			baseset::share_ptr<component> com = create_component(sClassName);
			if (com == 0)
			{
				res = false;
				cmp = cmp->NextSiblingElement("component");
				continue;
			}
			if (!com->load_from_xml(cmp))
			{
				res = false;
				cmp = cmp->NextSiblingElement("component");
				continue;
			}
			if (!com->initialize())
			{
				res = false;
				cmp = cmp->NextSiblingElement("component");
				continue;
			}
			m_components << com;
			cmp = cmp->NextSiblingElement("component");
		}
		components = components->NextSiblingElement("components");
	}
	return res;
}

baseset::share_ptr<demo_core::component> demo_core::application::get_component(const QString & class_name) const
{
	for (int i=0;i<m_components.size();++i)
	{
		if (m_components[i]->get_class_name() == class_name)
		{
			return m_components[i];
		}
	}
	return 0;
}

DEMO_CORE_EXPORT QString demo_core::get_full_path(const QString & path)
{
	if (path.indexOf(":") != -1)
	{
		return path;
	}
	else if (QFile::exists(QApplication::applicationDirPath() + "/" + path))
	{
		return QApplication::applicationDirPath() + "/" + path;
	}
	else
	{
		return path;
	}
}
