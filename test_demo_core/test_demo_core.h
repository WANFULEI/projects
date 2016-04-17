#ifndef TEST_DEMO_CORE_H
#define TEST_DEMO_CORE_H

#include <QtGui/QMainWindow>
#include "ui_test_demo_core.h"
#include "../demo_core/demo_core.h"

class test_demo_core : public QMainWindow , public demo_core::application
{
	Q_OBJECT

public:
	test_demo_core(QWidget *parent = 0);
	~test_demo_core();

	bool initialize();
	virtual bool load_from_xml(TiXmlElement * pXmlNode);
	void load_menu_bar();
	void load_tool_bar();

	QObject * load_menu_or_action(TiXmlElement * xml_node);
	Qt::Orientation get_orientation(const QString & s) const;
	Qt::ToolButtonStyle get_tool_button_style(const QString & s) const;
	QAction * get_action(const QString & action_id) const;

private:
	Ui::test_demo_coreClass ui;
	QList<QAction *> m_actions;

protected slots:
	void action_triggered();
};

#endif // TEST_DEMO_CORE_H
