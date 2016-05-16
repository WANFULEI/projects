#include "frameapp.h"
#include <QtGui/QComboBox>
#include <QToolButton>
#include "../frameutil/frameutil.h"
#include <QDockWidget>
#include "../framegui/framegui.h"
#include "../../base/baseset2/baseset2.h"
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QToolButton>
#include <QSpinBox>


frameapp::frameapp(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_tab = new QTabWidget;
	setCentralWidget(m_tab);

	framegui::framegui::get_instance()->set_main_window(this);
	
// 	m_map2d = new map2d;
// 	m_tab->addTab(m_map2d,QIcon(""),"二维地图");
// //	m_tab->addTab(new QWidget,QIcon(""),"二维地图");
// 	
// 	m_map3d = new map3d;
// 	m_tab->addTab(m_map3d,QIcon(""),"三维地图");

	initialize();
}

frameapp::~frameapp()
{

}

bool frameapp::load_from_xml(TiXmlElement * pXmlNode)
{
	return framecore::application::load_from_xml(pXmlNode);
}

bool frameapp::initialize()
{
	TiXmlDocument doc;
	if (!doc.LoadFile((qApp->applicationDirPath() + "/config/application.cfg").toStdString().c_str()))
	{
		return false;
	}
	bool res = load_from_xml(doc.RootElement());
	load_menu_bar();
	load_tool_bar();
	load_window();
	load_stautsbar();

	baseset::share_list_vector_manager<framecore::component> coms = get_components();
	for (int i=0;i<coms.size();++i)
	{
		if (coms[i] == 0)
		{
			continue;
		}
		coms[i]->initialize();
	}

	return res;
}

void frameapp::load_menu_bar()
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

QObject * frameapp::load_menu_or_action(TiXmlElement * xml_node)
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
		QString id = xml_node->Attribute("id");
		QAction * action = get_action(id);
		if (action == 0)
		{
			action = new QAction(QIcon(xml_node->Attribute("icon")),xml_node->Attribute("text"),0);
			QVariantHash data;
			data["handle_component_class_name"] = xml_node->Attribute("handle_component_class_name");
			data["id"] = id;
			action->setData(data);
			action->setObjectName(id);
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

void frameapp::action_triggered()
{
	QAction * action = dynamic_cast<QAction *>(sender());
	if (action == 0)
	{
		return;
	}

	QStringList handle_components = action->data().toHash()["handle_component_class_name"].toString().split('|');
	for (int i=0;i<handle_components.size();++i)
	{
		baseset::share_ptr<framecore::component> com = get_component(handle_components[i]);
		if (com == 0)
		{
			return;
		}
		com->action_triggered(action);
	}
}

void frameapp::load_tool_bar()
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
		QString toolbar_id = xml_node->Attribute("id");

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
// 			else if (QString(unknown->Value()).toLower() == "combobox")
// 			{
// 				QComboBox * combo = new QComboBox;
// 				TiXmlElement * info = unknown->FirstChildElement("item");
// 				QString id = unknown->Attribute("id");
// 				while (info)
// 				{
// 					combo->addItem(QIcon(info->Attribute("icon")),info->Attribute("text"));
// 					info = info->NextSiblingElement("item");
// 				}
// 				toolbar->addWidget(combo);
// 				QString handle_component_class_name = unknown->Attribute("handle_component_class_name");
// 				QStringList handle_component_class_names = handle_component_class_name.split('|');
// 				for (int i=0;i<handle_component_class_names.size();++i)
// 				{
// 					baseset::share_ptr<framecore::component> com = get_component(handle_component_class_names[i]);
// 					if (com)
// 					{
// 						com->on_create_control(id,combo);
// 					}
// 				}
// 			}
// 			else if (QString(unknown->Value()).toLower() == "toolbutton")
// 			{
// 				
// 				QString id = unknown->Attribute("id");
// 				QString icon = unknown->Attribute("icon");
// 				QString text = unknown->Attribute("text");
// 				QString tip = unknown->Attribute("tip");
// 				QToolButton * button = new QToolButton;
// 				button->setIcon(QIcon(frameutil::get_full_path(icon)));
// 				button->setText(text);
// 				button->setToolTip(tip);
// 				button->setObjectName(id);
// 				toolbar->addWidget(button);
// 
// 				QString handle_component_class_name = unknown->Attribute("handle_component_class_name");
// 				QStringList handle_component_class_names = handle_component_class_name.split('|');
// 				for (int i=0;i<handle_component_class_names.size();++i)
// 				{
// 					baseset::share_ptr<framecore::component> com = get_component(handle_component_class_names[i]);
// 					if (com)
// 					{
// 						com->on_create_control(id,button);
// 					}
// 				}
// 			}
// 			else if (QString(unknown->Value()).toLower() == "widget")
// 			{
// 				QString id = unknown->Attribute("id");
// 				baseset::share_ptr<framecore::component> com = get_component(unknown->Attribute("create_component_class_name"));
// 				if (com)
// 				{
// 					QWidget * widget = com->create_control(id);
// 					if (widget)
// 					{
// 						widget->setObjectName(id);
// 						toolbar->addWidget(widget);
// 						QString handle_component_class_name = unknown->Attribute("handle_component_class_name");
// 						QStringList handle_component_class_names = handle_component_class_name.split('|');
// 						for (int i=0;i<handle_component_class_names.size();++i)
// 						{
// 							baseset::share_ptr<framecore::component> com = get_component(handle_component_class_names[i]);
// 							if (com)
// 							{
// 								com->on_create_control(id,widget);
// 							}
// 						}
// 					}
// 				}
// 			}
			else
			{
				QWidget * widget = load_widget(unknown);
				if (widget)
				{
					toolbar->addWidget(widget);
				}
			}
			unknown = unknown->NextSiblingElement();
		}
		addToolBar(toolbar);
		xml_node = xml_node->NextSiblingElement("toolbar");
	}
}

Qt::Orientation frameapp::get_orientation(const QString & s) const
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

Qt::ToolButtonStyle frameapp::get_tool_button_style(const QString & s) const
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

QAction * frameapp::get_action(const QString & id) const
{
	for (int i=0;i<m_actions.size();++i)
	{
		if (m_actions[i]->data().toHash()["id"].toString() == id)
		{
			return m_actions[i];
		}
	}
	return 0;
}

void frameapp::load_window()
{
	TiXmlDocument doc;
	if (!doc.LoadFile((qApp->applicationDirPath() + "/config/window.cfg").toStdString().c_str()))
	{
		return;
	}
	TiXmlElement * root = doc.RootElement();
	if (root == 0)
	{
		return;
	}
	TiXmlElement * ele = root->FirstChildElement("window");
	while (ele)
	{
		QString id = ele->Attribute("id");
		QString icon = ele->Attribute("icon");
		QString tip = ele->Attribute("tip");
		QString dock = ele->Attribute("dock");
		QString windowTitle = ele->Attribute("windowTitle");
		QString allowedAreas = ele->Attribute("allowedAreas");
		QString floating = ele->Attribute("floating");
		QString dockArea = ele->Attribute("dockArea");

		QWidget * widget = 0;
		if (frameutil::get_bool_from_string(dock))
		{
			QDockWidget * dockwidget = new QDockWidget(windowTitle);
			dockwidget->setAllowedAreas(Qt::DockWidgetAreas(allowedAreas.toInt()));
			dockwidget->setFloating(frameutil::get_bool_from_string(floating));
			addDockWidget(Qt::DockWidgetArea(dockArea.toInt()),dockwidget);
			widget = dockwidget;
		}
		else
		{
			widget = new framegui::center_widget;
			m_tab->addTab(widget,QIcon(frameutil::get_full_path(icon)),windowTitle);
		}
		widget->setWindowTitle(windowTitle);
		widget->setObjectName(id);
		widget->setWindowIcon(QIcon(frameutil::get_full_path(icon)));
		widget->setToolTip(tip);
		framegui::framegui::get_instance()->add_object(widget);

		ele = ele->NextSiblingElement("window");
	}
}

void frameapp::load_stautsbar()
{
	TiXmlDocument doc;
	if (!doc.LoadFile((qApp->applicationDirPath() + "/config/statusbar.cfg").toStdString().c_str()))
	{
		return;
	}
	TiXmlElement * root = doc.RootElement();
	if (root == 0)
	{
		return;
	}
	TiXmlElement * item = root->FirstChildElement();
	while (item)
	{
		QWidget * widget = load_widget(item);
		if (widget)
		{
			statusBar()->addPermanentWidget(widget);
		}
		item = item->NextSiblingElement();
	}
}

QWidget * frameapp::load_widget(TiXmlElement * xml_node)
{
	QWidget * res = 0;
	if (xml_node == 0)
	{
		return res;
	}
	QString tag = xml_node->Value();
	QString id = xml_node->Attribute("id");
	QString text = xml_node->Attribute("text");
	QString icon = xml_node->Attribute("icon");
	QString handle_component_class_name = xml_node->Attribute("handle_component_class_name");
	QStringList handle_component_class_names = handle_component_class_name.split('|',QString::SkipEmptyParts);
	if (tag == "label")
	{
		res = new QLabel(text,0);
	}
	else if (tag == "lineedit")
	{
		QLineEdit * edt = new QLineEdit(text,0);
		QString width = xml_node->Attribute("width");
		int w = width.toInt();
		//edt->resize(w > 0 ? w : 120 , edt->height() );
		edt->setFixedWidth(w > 0 ? w : 120);
		res = edt;
	}
	else if (tag == "combobox")
	{
		QComboBox * cmb = new QComboBox(0);
		TiXmlElement * item = xml_node->FirstChildElement("item");
		while (item)
		{
			QString text,icon;
			text = item->Attribute("text");
			icon = item->Attribute("icon");
			cmb->addItem(QIcon(frameutil::get_full_path(icon)),text);
			item = item->NextSiblingElement("item");
		}
		res = cmb;
	}
	else if (tag == "spinbox")
	{
		QSpinBox * spn = new QSpinBox(0);
		res = spn;
	}
	else if (tag == "checkbox")
	{
		QCheckBox * chk = new QCheckBox(text,0);
		res = chk;
	}
	else if (tag == "pushbutton")
	{
		QPushButton * btn = new QPushButton(QIcon(frameutil::get_full_path(icon)),text,0);
		res = btn;
	}
	else if (tag == "toolbutton")
	{
		QToolButton * btn = new QToolButton(0);
		btn->setIcon(QIcon(frameutil::get_full_path(icon)));
		btn->setText(text);
		res = btn;
	}
	else if (tag == "widget")
	{
		QString create_component_class_name = xml_node->Attribute("create_component_class_name");
		auto com = get_component(create_component_class_name);
		if (com)
		{
			res = com->create_control(id);
		}
	}
	else
	{

	}
	if (res)
	{
		res->setObjectName(id);
		for (int i=0;i<handle_component_class_names.size();++i)
		{
			auto com = get_component(handle_component_class_names[i]);
			if (com)
			{
				com->on_create_control(id,res);
			}
		}
	}
	return res;
}

