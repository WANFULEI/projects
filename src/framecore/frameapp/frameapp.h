#ifndef FRAMEAPP_H
#define FRAMEAPP_H

#include <QtGui/QMainWindow>
#include "ui_frameapp.h"
#include "framecore/framecore.h"

class frameapp : public QMainWindow , public framecore::application
{
	Q_OBJECT

public:
	frameapp(QWidget *parent = 0);
	~frameapp();

	bool initialize();
	virtual bool load_from_xml(TiXmlElement * pXmlNode);
	void load_menu_bar();
	void load_tool_bar();
	void load_window();
	void load_stautsbar();

	QObject * load_menu_or_action(TiXmlElement * xml_node);
	QWidget * load_widget(TiXmlElement * xml_node);
	Qt::Orientation get_orientation(const QString & s) const;
	Qt::ToolButtonStyle get_tool_button_style(const QString & s) const;
	QAction * get_action(const QString & action_id) const;

private:
	Ui::test_framecoreClass ui;
	QList<QAction *> m_actions;
	QTabWidget * m_tab;

protected slots:
	void action_triggered();
};

#endif // FRAMEAPP_H
