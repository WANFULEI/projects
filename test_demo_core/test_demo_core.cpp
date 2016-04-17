#include "test_demo_core.h"
#include <QtGui/QComboBox>

test_demo_core::test_demo_core(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	initialize();
}

test_demo_core::~test_demo_core()
{

}

bool test_demo_core::load_from_xml(TiXmlElement * pXmlNode)
{
	return demo_core::application::load_from_xml(pXmlNode);
}

bool test_demo_core::initialize()
{
	TiXmlDocument doc;
	if (!doc.LoadFile((qApp->applicationDirPath() + "/config/application.cfg").toStdString().c_str()))
	{
		return false;
	}
	load_menu_bar();
	load_tool_bar();
	return load_from_xml(doc.RootElement());
}

void test_demo_core::load_menu_bar()
{
	TiXmlDocument doc;
	if (!doc.LoadFile((qApp->applicationDirPath() + "/config/menu.cfg").toStdString().c_str()))
	{
		return;
	}
	TiXmlElement * xml_node = doc.RootElement();
	if (xml_node == 0)
	{
		return;
	}
	xml_node = xml_node->FirstChildElement();
	while (xml_node)
	{
		if (QString(xml_node->Value()).toLower() == "menu")
		{
			menuBar()->addMenu(dynamic_cast<QMenu *>(load_menu_or_action(xml_node)));
		}
		else if (QString(xml_node->Value()).toLower() == "action")
		{
			menuBar()->addAction(dynamic_cast<QAction *>(load_menu_or_action(xml_node)));
		}
		else
		{

		}
		xml_node = xml_node->NextSiblingElement();
	}
	return;
}

QObject * test_demo_core::load_menu_or_action(TiXmlElement * xml_node)
{
	if (xml_node == 0)
	{
		return 0;
	}
	if (QString(xml_node->Value()).toLower() == "menu")
	{
		QMenu * menu = new QMenu(xml_node->Attribute("text"),0);
		xml_node = xml_node->FirstChildElement();
		while (xml_node)
		{
			QObject * unknown = load_menu_or_action(xml_node);
			if (dynamic_cast<QAction *>(unknown))
			{
				menu->addAction(dynamic_cast<QAction *>(unknown));
			}
			else if (dynamic_cast<QMenu *>(unknown))
			{
				menu->addMenu(dynamic_cast<QMenu *>(unknown));
			}
			else
			{

			}
			xml_node = xml_node->NextSiblingElement();
		}
		return menu;
	}
	else if (QString(xml_node->Value()).toLower() == "action")
	{
		QAction * action = get_action(xml_node->Attribute("action_id"));
		if (action == 0)
		{
			action = new QAction(QIcon(xml_node->Attribute("icon")),xml_node->Attribute("text"),0);
			QVariantHash data;
			data["handle_component_class_name"] = xml_node->Attribute("handle_component_class_name");
			data["action_id"] = xml_node->Attribute("action_id");
			action->setData(data);
			connect(action,SIGNAL(triggered()),this,SLOT(action_triggered()));
			m_actions << action;
		}
		return action;
	}
	else
	{
		return 0;
	}
}

void test_demo_core::action_triggered()
{
	QAction * action = dynamic_cast<QAction *>(sender());
	if (action == 0)
	{
		return;
	}
	baseset::share_ptr<demo_core::component> com = get_component(action->data().toHash()["handle_component_class_name"].toString());
	if (com == 0)
	{
		return;
	}
	com->action_triggered(action);
}

void test_demo_core::load_tool_bar()
{

	TiXmlDocument doc;
	if (!doc.LoadFile((qApp->applicationDirPath() + "/config/toolbar.cfg").toStdString().c_str()))
	{
		return;
	}
	TiXmlElement * xml_node = doc.RootElement();
	if (xml_node == 0)
	{
		return;
	}
	xml_node = xml_node->FirstChildElement("toolbar");
	while (xml_node)
	{
		QToolBar * toolbar = new QToolBar;
		QString allowed_areas = xml_node->Attribute("allowed_areas");
		toolbar->setAllowedAreas((Qt::ToolBarAreas)allowed_areas.toInt());
		int n;
		xml_node->Attribute("movable",&n);
		toolbar->setMovable(n);
		xml_node->Attribute("floatable",&n);
		toolbar->setFloatable(n);
		toolbar->setOrientation(get_orientation(xml_node->Attribute("orientation")));
		toolbar->setToolButtonStyle(get_tool_button_style(xml_node->Attribute("tool_button_style")));
		xml_node->Attribute("icon_size",&n);
		toolbar->setIconSize(QSize(n,n));
		TiXmlElement * unknown = xml_node->FirstChildElement();
		while (unknown)
		{
			if (QString(unknown->Value()).toLower() == "action")
			{
				toolbar->addAction(dynamic_cast<QAction *>(load_menu_or_action(unknown)));
			}
			else if (QString(unknown->Value()).toLower() == "combobox")
			{
				QComboBox * combo = new QComboBox;
				TiXmlElement * info = unknown->FirstChildElement("item");
				while (info)
				{
					combo->addItem(QIcon(info->Attribute("icon")),info->Attribute("text"));
					info = info->NextSiblingElement("item");
				}
				toolbar->addWidget(combo);
			}
			else if (QString(unknown->Value()).toLower() == "widget")
			{
				baseset::share_ptr<demo_core::component> com = get_component(unknown->Attribute("create_component_class_name"));
// 				if (com)
// 				{
// 					com->
// 				}
			}
			else
			{

			}
			unknown = unknown->NextSiblingElement();
		}
		addToolBar(toolbar);
		xml_node = xml_node->NextSiblingElement("toolbar");
	}
}

Qt::Orientation test_demo_core::get_orientation(const QString & s) const
{
	Qt::Orientation res = Qt::Horizontal;
	if (s.toLower() == "qt::horizontal" || s.toLower() == "horizontal" || s == "1")
	{
		res = Qt::Horizontal;
	}
	else if (s.toLower() == "qt::vertical" || s.toLower() == "vertical" || s == "2")
	{
		res = Qt::Vertical;
	}
	else
	{

	}
	return res;
}

Qt::ToolButtonStyle test_demo_core::get_tool_button_style(const QString & s) const
{
	Qt::ToolButtonStyle res;
	if (s.toLower() == "qt::toolbuttonicononly" || s.toLower() == "toolbuttonicononly" || s == "0")
	{
		res = Qt::ToolButtonIconOnly;
	}
	else if (s.toLower() == "qt::toolbuttontextonly" || s.toLower() == "toolbuttontextonly" || s == "1")
	{
		res = Qt::ToolButtonTextOnly;
	}
	else if (s.toLower() == "qt::toolbuttontextbesideicon" || s.toLower() == "toolbuttontextbesideicon" || s == "2")
	{
		res = Qt::ToolButtonTextBesideIcon;
	}
	else if (s.toLower() == "qt::toolbuttontextundericon" || s.toLower() == "toolbuttontextundericon" || s == "3")
	{
		res = Qt::ToolButtonTextUnderIcon;
	}
	else if (s.toLower() == "qt::toolbuttonfollowstyle" || s.toLower() == "toolbuttonfollowstyle" || s == "4")
	{
		res = Qt::ToolButtonFollowStyle;
	}
	else
	{

	}
	return res;
}

QAction * test_demo_core::get_action(const QString & action_id) const
{
	for (int i=0;i<m_actions.size();++i)
	{
		if (m_actions[i]->data().toHash()["action_id"].toString() == action_id)
		{
			return m_actions[i];
		}
	}
	return 0;
}

