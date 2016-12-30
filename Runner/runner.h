#ifndef RUNNER_H
#define RUNNER_H

#include <QtGui/QMainWindow>
#include "ui_runner.h"
#include <Ribbon/include/QtitanRibbon.h>
#include <tinyxml.h>
#include <QCheckBox>
#include <QRadioButton>

class Runner : public RibbonMainWindow
{
	Q_OBJECT

public:
	Runner(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Runner();

private:
	QSize sizeHint() const;

private:
	void loadUIFromXml(TiXmlElement *xmlNode);
	bool isUse(TiXmlElement *xmlNode);
	void connectSignals(TiXmlElement *xmlNode, QObject *sender, QString defaultSignal = "");
	void loadItems(TiXmlElement *xmlNode, RibbonGroup *group);
	QAction *loadAction(TiXmlElement *xmlNode);
	QAction::Priority toActionPriority(QString s);
	Qt::ShortcutContext toShortcutContext(QString s);
	Qt::ToolButtonStyle toToolButtonStyle(QString s);
	QMenu *loadMenu(TiXmlElement *xmlNode);
	Qt::CursorShape toCursorShape(QString s);
	Qt::FocusPolicy toFocusPolicy(QString s);
	void loadWidget(QWidget *widget, TiXmlElement *xmlNode);
	QCheckBox *loadCheckBox(TiXmlElement *xmlNode);
	Qt::CheckState toCheckState(QString s);
	void loadAbstractButton(QAbstractButton *button, TiXmlElement *xmlNode);
	QRadioButton *loadRadioButton(TiXmlElement *xmlNode);

private slots:
	void slot_test();
};

#endif // RUNNER_H
