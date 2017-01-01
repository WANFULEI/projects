#ifndef RUNNER_H
#define RUNNER_H

#include <QtGui/QMainWindow>
#include "ui_runner.h"
#include <Ribbon/include/QtitanRibbon.h>
#include <Ribbon/include/QtnRibbonQuickAccessBar.h>
#include <tinyxml.h>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QFontComboBox>
#include <QProgressBar>
#include <QScrollBar>
#include <QSlider>

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
	QObject *loadItem(TiXmlElement *itemNode);

	void loadWidget(QWidget *widget, TiXmlElement *xmlNode);
	void loadAbstractButton(QAbstractButton *button, TiXmlElement *xmlNode);
	void loadAbstractSlider(QAbstractSlider *abstractSlider, TiXmlElement *xmlNode);

	QAction *loadAction(TiXmlElement *xmlNode);
	QMenu *loadMenu(TiXmlElement *xmlNode);
	QCheckBox *loadCheckBox(TiXmlElement *xmlNode);
	QRadioButton *loadRadioButton(TiXmlElement *xmlNode);
	QLineEdit *loadLineEdit(TiXmlElement *xmlNode);
	QComboBox *loadComboBox(TiXmlElement *xmlNode);
	QSpinBox *loadSpinBox(TiXmlElement *xmlNode);
	QFontComboBox *loadFontComboBox(TiXmlElement *xmlNode);
	QLabel *loadLable(TiXmlElement *xmlNode);
	QProgressBar *loadProgressBar(TiXmlElement *xmlNode);
	QScrollBar *loadScrollBar(TiXmlElement *xmlNode);
	QSlider *loadSlider(TiXmlElement *xmlNode);
	RibbonSliderPane *loadRibbonSliderPane(TiXmlElement *xmlNode);

	Qt::Alignment toAlignment(QString s);
	Qt::CheckState toCheckState(QString s);
	Qt::ToolButtonStyle toToolButtonStyle(QString s);
	QFontComboBox::FontFilters toFontFilters(QString s);
	Qt::Orientation toOrientation(QString s);
	QSlider::TickPosition toTickPosition(QString s);

	void createOptions();

private slots:
	void options(QAction* action);
	void optionsFont(QAction* act);
	void slot_test();

private:
	int m_defaultFont;
	QAction *m_actionDefault;
};

#endif // RUNNER_H
