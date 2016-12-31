#include "runner.h"
#include "myLog.h"
#include "commonFunctions.h"
#include <QStackedWidget>
#include <QMessageBox>
#include <QApplication>
#include <qdesktopwidget.h>

#define Prop_ContextColor   "ContextColor"

#define Font_Default  "System Default"
#define Font_Normal   "Normal"
#define Font_Large    "Large"
#define Font_ExLarge  "Extra Large"


Runner::Runner(QWidget *parent, Qt::WFlags flags)
	: RibbonMainWindow(parent, flags)
{
	m_defaultFont = 8;
	m_actionDefault = 0;
	setObjectName("me");
	MyLog::Init("log");

	TiXmlDocument doc;
	if(doc.LoadFile("config.xml")){
		loadUIFromXml(doc.FirstChildElement("Ribbon"));
	}else{
		LOG_ERROR << "配置文件config.xml不存在或格式不正确！";
	}

	ribbonBar()->setFrameThemeEnabled(true);
	m_defaultFont = ribbonBar()->font().pointSize();
}

Runner::~Runner()
{

}

void Runner::loadUIFromXml(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return;
	if(xmlNode->Value() != QString("Ribbon")) return;

	ribbonBar()->addSystemButton(QIcon(xmlNode->Attribute("Logo")), xmlNode->Attribute(""));

	TiXmlElement *childNode = xmlNode->FirstChildElement();
	while(childNode){
		if(!isUse(childNode)){
			childNode = childNode->NextSiblingElement();
			continue;
		}
		if(childNode->Value() == QString("Page")){
			RibbonPage *ribbonPage = ribbonBar()->addPage(childNode->Attribute("Name"));
			TiXmlElement *groupNode = childNode->FirstChildElement("Group");
			while(groupNode){
				if(!isUse(groupNode)){
					groupNode = groupNode->NextSiblingElement("Group");
					continue;
				}
				RibbonGroup *ribbonGroup = ribbonPage->addGroup(groupNode->Attribute("Name"));
				if(getAttribute(groupNode, "OptionButtonVisible", false)){
					ribbonGroup->setOptionButtonVisible(true);
					QAction *act = ribbonGroup->getOptionButtonAction();
					act->setToolTip(getAttribute(groupNode, "ToolTip"));
					connectSignals(groupNode, act, "triggered()");
				}
				ribbonGroup->setControlsCentering(getAttribute(groupNode, "ControlsCentering", false));
				ribbonGroup->setControlsGrouping(getAttribute(groupNode, "ControlsGrouping", false));
				loadItems(groupNode, ribbonGroup);
				groupNode = groupNode->NextSiblingElement("Group");
			}
		}else if(childNode->Value() == QString("Item")){
			QString type = childNode->Attribute("Type");
			if(type.toLower() == "action"){
				QAction *action = ribbonBar()->addAction(QIcon(childNode->Attribute("Icon")), childNode->Attribute("Text"), toToolButtonStyle(childNode->Attribute("ToolButtonStyle")));
				connectSignals(childNode, action, "triggered()");
			}else if(type.toLower() == "menu"){
				QMenu *menu = loadMenu(childNode);
				ribbonBar()->addAction(QIcon(childNode->Attribute("Icon")), childNode->Attribute("Text"), toToolButtonStyle(childNode->Attribute("ToolButtonStyle")), menu);
			}else if(type.toLower() == "options"){
				createOptions();
			}else if(type.toLower() == "quickaccess"){
				RibbonQuickAccessBar* quickAccessBar = ribbonBar()->getQuickAccessBar();
				QAction* action = quickAccessBar->actionCustomizeButton();
				action->setToolTip(tr("Customize Quick Access Bar"));
				TiXmlElement *actionNode = childNode->FirstChildElement("Item");
				while(actionNode){
					QAction *action = loadAction(actionNode);
					if(action) quickAccessBar->addAction(action);
					actionNode = actionNode->NextSiblingElement("Item");
				}
				ribbonBar()->showQuickAccess(true);
			}
		}
		childNode = childNode->NextSiblingElement();
	}
}

QSize Runner::sizeHint() const
{
	return QSize(800, 600);
}

bool Runner::isUse(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return false;
	return getAttribute(xmlNode, "Use", true);
}

void Runner::connectSignals(TiXmlElement *xmlNode, QObject *sender, QString defaultSignal){
	if(xmlNode == 0 || sender == 0) return;
	TiXmlElement *connectionNode = xmlNode->FirstChildElement("Connection");
	while(connectionNode){
		if(!isUse(connectionNode)){
			connectionNode = connectionNode->NextSiblingElement("Connection");	
			continue;
		}
		QString receiverName = getAttribute(connectionNode, "Receiver");
		if(receiverName.isEmpty()){
			LOG_INFO << "row=" << QString::number(connectionNode->Row()).toStdString() << ",receiver is empty";
			connectionNode = connectionNode->NextSiblingElement("Connection");
 			continue;
		}
		QString signalName = getAttribute(connectionNode, "Signal", defaultSignal);
		if(signalName.isEmpty()){
			LOG_INFO << "row=" << QString::number(connectionNode->Row()).toStdString().c_str() << ",signal is empty";
			connectionNode = connectionNode->NextSiblingElement("Connection");	
			continue;
		}
		QString slotName = getAttribute(connectionNode, "Slot");
		if(slotName.isEmpty()){
			LOG_INFO << "row=" << QString::number(connectionNode->Row()).toStdString() << ",slot is empty";
			connectionNode = connectionNode->NextSiblingElement("Connection");	
			continue;
		}
		QList<QObject *> objs = qApp->findChildren<QObject *>(receiverName);
		objs += findChildren<QObject *>(receiverName);
		if(receiverName == objectName()) objs << this;
		if(objs.size() == 0){
			LOG_INFO << "no receiver name=" << receiverName.toStdString().c_str();
		}
		for(int i=0; i<objs.size(); ++i){
			if(!QObject::connect(sender, ("2"+signalName).toStdString().c_str(), objs[i], ("1"+slotName).toStdString().c_str())){
				LOG_INFO << "row=" << QString::number(connectionNode->Row()).toStdString() <<  ",connect failed, sender=" << sender->objectName().toStdString() << ",receiver=" << receiverName.toStdString() << ",signal=" << signalName.toStdString() << ",slot=" << slotName.toStdString();
			}
		}
		connectionNode = connectionNode->NextSiblingElement("Connection");	
	}
}

void Runner::loadItems(TiXmlElement *xmlNode, RibbonGroup *group){
	if(xmlNode == 0 || group == 0) return;
	TiXmlElement *itemNode = xmlNode->FirstChildElement("Item");
	while(itemNode){
		QString type = QString(itemNode->Attribute("Type")).toLower();
		if(type == "action"){
			QAction *action = loadAction(itemNode);
			if(action){
				group->addAction(action, toToolButtonStyle(itemNode->Attribute("ToolButtonStyle")));
			}
		}else if(type == "menu"){
			QMenu *menu = loadMenu(itemNode);
			if(menu) group->addAction(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), 
				toToolButtonStyle(itemNode->Attribute("ToolButtonStyle")), menu);
		}else if(type == "checkbox"){
			QCheckBox *checkBox = loadCheckBox(itemNode);
			if(checkBox) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), checkBox);
		}else if(type == "spinbox"){
			QSpinBox *spinBox = loadSpinBox(itemNode);
			if(spinBox) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), spinBox);
		}else if(type == "separator"){
			group->addSeparator();
		}else if(type == "radiobutton"){
			QRadioButton *radioButton = loadRadioButton(itemNode);
			if(radioButton) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), radioButton);
		}else if(type == "combobox"){
			QComboBox *comboBox = loadComboBox(itemNode);
			if(comboBox) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), comboBox);
		}else if(type == "lineedit"){
			QLineEdit *lineEdit = loadLineEdit(itemNode);
			if(lineEdit) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), lineEdit);
		}else if(type == "fontcombobox"){
			QFontComboBox *comboBox = loadFontComboBox(itemNode);
			if(comboBox) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), comboBox);
		}else if(type == "label"){
			QLabel *label = loadLable(itemNode);
			if(label) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), label);
		}else if(type == "progressbar"){
			QProgressBar *progressBar = loadProgressBar(itemNode);
			if(progressBar) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), progressBar);
		}else if(type == "scrollbar"){
			QScrollBar *scrollBar = loadScrollBar(itemNode);
			if(scrollBar) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), scrollBar);
		}else if(type == "slider"){
			QSlider *slider = loadSlider(itemNode);
			if(slider) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), slider);
		}else if(type == "ribbonsliderpane"){
			RibbonSliderPane *slider = loadRibbonSliderPane(itemNode);
			if(slider) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), slider);
		}
		itemNode = itemNode->NextSiblingElement("Item");
	}
}

QAction *Runner::loadAction(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "action") return 0;
	QAction *action = new QAction(this);
	action->setObjectName(xmlNode->Attribute("ObjectName"));
	action->setIcon(QIcon(xmlNode->Attribute("Icon")));
	action->setText(xmlNode->Attribute("Text"));
	action->setToolTip(xmlNode->Attribute("ToopTip"));
	bool visible = getAttribute(xmlNode, "Visible", true);
	if(!visible) action->setVisible(false);
	action->setStatusTip(xmlNode->Attribute("StatusTip"));
	if(getAttribute(xmlNode, "Checkable", false)){
		action->setCheckable(true);
		action->setChecked(getAttribute(xmlNode, "Checked", false));
	}
	action->setEnabled(getAttribute(xmlNode, "Enabled", true));
	action->setIconText(xmlNode->Attribute("IconText"));
	action->setShortcut(QKeySequence(xmlNode->Attribute("Shortcut")));
	action->setWhatsThis(xmlNode->Attribute("WhatsThis"));
	connectSignals(xmlNode, action, "triggered()");
	return action;
}

Qt::ToolButtonStyle Runner::toToolButtonStyle(QString s){
	s = s.toLower();
	if(s == "qt::toolbuttonicononly" || s == "toolbuttonicononly" || s == "0") return Qt::ToolButtonIconOnly;
	else if(s == "qt::toolbuttontextonly" || s == "toolbuttontextonly" || s == "1") return Qt::ToolButtonTextOnly;
	else if(s == "qt::toolbuttontextbesideicon" || s == "toolbuttontextbesideicon" || s == "2") return Qt::ToolButtonTextBesideIcon;
	else if(s == "qt::toolbuttontextundericon" || s == "toolbuttontextundericon" || s == "2") return Qt::ToolButtonTextUnderIcon;
	else if(s == "qt::toolbuttonfollowstyle" || s == "toolbuttonfollowstyle" || s == "2") return Qt::ToolButtonFollowStyle;
	else return Qt::ToolButtonTextUnderIcon;
}

void Runner::slot_test(){
	QMessageBox::warning(this, "test", "test");
}


QMenu *Runner::loadMenu(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "menu") return 0;
	QMenu *menu = new QMenu(this);
	loadWidget(menu, xmlNode);
	menu->setIcon(QIcon(xmlNode->Attribute("Icon")));
	menu->setTitle(xmlNode->Attribute("Text"));
	TiXmlElement *childNode = xmlNode->FirstChildElement("Item");
	while(childNode){
		QString childType = childNode->Attribute("Type");
		if(childType.toLower() == "action"){
			QAction *action = loadAction(childNode);
			if(action) menu->addAction(action);
		}else if(childType.toLower() == "menu"){
			QMenu *childMenu = loadMenu(childNode);
			if(childMenu) menu->addMenu(childMenu);
		}
		childNode = childNode->NextSiblingElement("Item");
	}
	connectSignals(xmlNode, menu, "triggered ( QAction * )");
	return menu;
}

void Runner::loadWidget(QWidget *widget, TiXmlElement *xmlNode){
	if(widget == 0 || xmlNode == 0) return;
	widget->setObjectName(xmlNode->Attribute("ObjectName"));
	//int width = getAttribute(xmlNode, "Width", 0);
	//int height = getAttribute(xmlNode, "Height", 0);
	//if(width > 0 && height > 0) widget->resize(width, height);
	widget->setEnabled(getAttribute(xmlNode, "Enabled", true));
	int maxH = getAttribute(xmlNode, "MaximumHeight", 0);
	if(maxH > 0) widget->setMaximumHeight(maxH);
	int maxW = getAttribute(xmlNode, "MaximumWidth", 0);
	if(maxW > 0) widget->setMaximumWidth(maxW);
	int minH = getAttribute(xmlNode, "MinimumHeight", 0);
	if(minH > 0) widget->setMinimumHeight(minH);
	int minW = getAttribute(xmlNode, "MinimumWidth", 0);
	if(minW > 0) widget->setMinimumWidth(minW);
	widget->setStatusTip(xmlNode->Attribute("StatusTip"));
	widget->setStyleSheet(xmlNode->Attribute("StyleSheet"));
	widget->setToolTip(xmlNode->Attribute("ToolTip"));
	bool visible = getAttribute(xmlNode, "Visible", true);
	if(!visible) widget->setVisible(false);
	widget->setWhatsThis(xmlNode->Attribute("WhatsThis"));
	widget->setWindowIcon(QIcon(xmlNode->Attribute("WindowIcon")));
	widget->setWindowIconText(xmlNode->Attribute("WindowIconText"));
	widget->setWindowOpacity(getAttribute(xmlNode, "WindowOpacity", 1.0));
	widget->setWindowTitle(xmlNode->Attribute("WindowTitle"));
}

QCheckBox *Runner::loadCheckBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "checkbox") return 0;
	QCheckBox *checkBox = new QCheckBox(this);
	loadAbstractButton(checkBox, xmlNode);
	checkBox->setCheckState(toCheckState(xmlNode->Attribute("CheckState")));
	connectSignals(xmlNode, checkBox, "stateChanged ( int )");
	return checkBox;
}

Qt::CheckState Runner::toCheckState(QString s){
	s = s.toLower();
	if(s == "qt::unchecked" || s == "unchecked" || s == "0") return Qt::Unchecked;
	else if(s == "qt::partiallychecked" || s == "partiallychecked" || s == "1") return Qt::PartiallyChecked;
	else if(s == "qt::checked" || s == "checked" || s == "2") return Qt::Checked;
	else return Qt::Unchecked;
}

void Runner::loadAbstractButton(QAbstractButton *button, TiXmlElement *xmlNode){
	if(button == 0 || xmlNode == 0) return;
	loadWidget(button, xmlNode);
	button->setCheckable(getAttribute(xmlNode, "Checkable", true));
	button->setChecked(getAttribute(xmlNode, "Checked", false));
	button->setIcon(QIcon(xmlNode->Attribute("Icon")));
	button->setIconSize(QSize(getAttribute(xmlNode, "IconWidth", 24), getAttribute(xmlNode, "IconHeight", 24)));
	button->setText(xmlNode->Attribute("Text"));
}

QRadioButton *Runner::loadRadioButton(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "radiobutton") return 0;
	QRadioButton *radioButton = new QRadioButton(this);
	loadAbstractButton(radioButton, xmlNode);
	connectSignals(xmlNode, radioButton, "clicked ( bool checked )");
	return radioButton;
}

QLineEdit *Runner::loadLineEdit(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "lineedit") return 0;
	QLineEdit *lineEdit = new QLineEdit(this);
	loadWidget(lineEdit, xmlNode);
	lineEdit->setReadOnly(getAttribute(xmlNode, "ReadOnly", false));
	lineEdit->setAlignment(toAlignment(xmlNode->Attribute("Alignment")));
	lineEdit->setFrame(getAttribute(xmlNode, "Frame", true));
	lineEdit->setInputMask(xmlNode->Attribute("InputMask"));
	lineEdit->setMaxLength(getAttribute(xmlNode, "MaxLength", -1));
	connectSignals(xmlNode, lineEdit, "editingFinished ()");
	return lineEdit;
}

Qt::Alignment Runner::toAlignment(QString s){
	s = s.toLower();
	if(s == "qt::alignleft" || s == "alignleft" || s == "1") return Qt::AlignLeft;
	else if(s == "qt::alignright" || s == "alignright" || s == "2") return Qt::AlignRight;
	else if(s == "qt::alignhcenter" || s == "alignhcenter" || s == "4") return Qt::AlignHCenter;
	else if(s == "qt::alignjustify" || s == "alignjustify" || s == "8") return Qt::AlignJustify;
	else return Qt::Alignment(s.toInt());
}

QComboBox *Runner::loadComboBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "combobox") return 0;
	QComboBox *comboBox = new QComboBox(this);
	loadWidget(comboBox, xmlNode);
	comboBox->setEditable(getAttribute(xmlNode, "Editable", true));
	TiXmlElement *itemNode = xmlNode->FirstChildElement("Item");
	while(itemNode){
		comboBox->addItem(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), itemNode->Attribute("UserData"));
		itemNode = itemNode->NextSiblingElement("Item");
	}
	comboBox->setCurrentIndex(getAttribute(xmlNode, "CurrentIndex", 0));
	comboBox->setDuplicatesEnabled(getAttribute(xmlNode, "DuplicatesEnabled", true));
	comboBox->setFrame(getAttribute(xmlNode, "Frame", true));
	comboBox->setIconSize(QSize(getAttribute(xmlNode, "IconWidth", 24), getAttribute(xmlNode, "IconHeight", 24)));
	comboBox->setMaxCount(getAttribute(xmlNode, "MaxCount", -1));
	comboBox->setMaxVisibleItems(getAttribute(xmlNode, "MaxVisibleItems", 24));
	comboBox->setMinimumContentsLength(getAttribute(xmlNode, "MinimumContentsLength", 64));
	connectSignals(xmlNode, comboBox, "currentIndexChanged ( int )");
	return comboBox;
}

QSpinBox *Runner::loadSpinBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "spinbox") return 0;
	QSpinBox *spinBox = new QSpinBox(this);
	loadWidget(spinBox, xmlNode);
	spinBox->setMinimum(getAttribute(xmlNode, "Minimum", 0));
	spinBox->setMaximum(getAttribute(xmlNode, "Maximum", 100));
	spinBox->setPrefix(xmlNode->Attribute("Prefix"));
	spinBox->setSuffix(xmlNode->Attribute("Suffix"));
	spinBox->setSingleStep(getAttribute(xmlNode, "SingleStep", 1));
	spinBox->setValue(getAttribute(xmlNode, "Value", 0));
	connectSignals(xmlNode, spinBox, "valueChanged ( int )");
	return spinBox;
}

QFontComboBox *Runner::loadFontComboBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "fontcombobox") return 0;
	QFontComboBox *fontCombo = new QFontComboBox(this);
	loadWidget(fontCombo, xmlNode);
	//fontCombo->setEditable(getAttribute(xmlNode, "Editable", true));
	//fontCombo->setCurrentIndex(getAttribute(xmlNode, "CurrentIndex", 0));
	fontCombo->setDuplicatesEnabled(getAttribute(xmlNode, "DuplicatesEnabled", true));
	fontCombo->setFrame(getAttribute(xmlNode, "Frame", true));
	fontCombo->setIconSize(QSize(getAttribute(xmlNode, "IconWidth", 24), getAttribute(xmlNode, "IconHeight", 24)));
	//fontCombo->setMaxCount(getAttribute(xmlNode, "MaxCount", -1));
	fontCombo->setMaxVisibleItems(getAttribute(xmlNode, "MaxVisibleItems", 24));
	fontCombo->setMinimumContentsLength(getAttribute(xmlNode, "MinimumContentsLength", 64));

	fontCombo->setFontFilters(toFontFilters(xmlNode->Attribute("FontFilters")));
	connectSignals(xmlNode, fontCombo, "currentIndexChanged ( int )");
	return fontCombo;
}

QFontComboBox::FontFilters Runner::toFontFilters(QString s){
	s = s.toLower();
	if(s == "qfontcombobox::allfonts" || s == "allfonts" || s == "0") return QFontComboBox::AllFonts;
	else if(s == "qfontcombobox::scalablefonts" || s == "scalablefonts" || s == "1") return QFontComboBox::AllFonts;
	else if(s == "qfontcombobox::nonscalablefonts" || s == "nonscalablefonts" || s == "2") return QFontComboBox::AllFonts;
	else if(s == "qfontcombobox::monospacedfonts" || s == "monospacedfonts" || s == "4") return QFontComboBox::AllFonts;
	else if(s == "qfontcombobox::proportionalfonts" || s == "proportionalfonts" || s == "8") return QFontComboBox::AllFonts;
	else return QFontComboBox::FontFilters(s.toInt());
}

QLabel *Runner::loadLable(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "label") return 0;
	QLabel *label = new QLabel(this);
	loadWidget(label, xmlNode);
	label->setAlignment(toAlignment(xmlNode->Attribute("Alignment")));
	label->setOpenExternalLinks(getAttribute(xmlNode, "OpenExternalLinks", false));
	QPixmap pixmap = QPixmap(xmlNode->Attribute("Pixmap"));
	if(pixmap.isNull()) label->setText(xmlNode->Attribute("Text"));
	else label->setPixmap(QPixmap(xmlNode->Attribute("Pixmap")));
	label->setScaledContents(getAttribute(xmlNode, "ScaledContents", false));
	label->setWordWrap(getAttribute(xmlNode, "WordWrap", false));
	connectSignals(xmlNode, label, "linkActivated ( const QString & )");
	return label;
}

QProgressBar *Runner::loadProgressBar(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "progressbar") return 0;
	QProgressBar *progressBar = new QProgressBar(this);
	loadWidget(progressBar, xmlNode);
	//Alignment="" Minimum="" Maximum="" Orientation="" TextVisible="true" Value="" 
	progressBar->setAlignment(toAlignment(xmlNode->Attribute("Alignment")));
	progressBar->setMinimum(getAttribute(xmlNode, "Minimum", 0));
	progressBar->setMaximum(getAttribute(xmlNode, "Maximum", 100));
	progressBar->setOrientation(toOrientation(xmlNode->Attribute("Orientation")));
	progressBar->setTextVisible(getAttribute(xmlNode, "TextVisible", true));
	progressBar->setValue(getAttribute(xmlNode, "Value", 0));
	connectSignals(xmlNode, progressBar, "valueChanged ( int )");
	return progressBar;
}

Qt::Orientation Runner::toOrientation(QString s){
	s = s.toLower();
	if(s == "qt::horizontal" || s == "horizontal" || s == "1") return Qt::Horizontal;
	else if(s == "qt::vertical" || s == "vertical" || s == "2") return Qt::Vertical;
	else return Qt::Horizontal;
}

QScrollBar *Runner::loadScrollBar(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "scrollbar") return 0;
	QScrollBar *scrollBar = new QScrollBar(this);
	loadAbstractSlider(scrollBar, xmlNode);
	connectSignals(xmlNode, scrollBar, "sliderReleased ()");
	return scrollBar;
}

void Runner::loadAbstractSlider(QAbstractSlider *abstractSlider, TiXmlElement *xmlNode){
	if(abstractSlider == 0 || xmlNode == 0) return;
	loadWidget(abstractSlider, xmlNode);
	abstractSlider->setMinimum(getAttribute(xmlNode, "Minimum", 0));
	abstractSlider->setMaximum(getAttribute(xmlNode, "Maximum", 100));
	abstractSlider->setOrientation(toOrientation(xmlNode->Attribute("Orientation")));
	abstractSlider->setPageStep(getAttribute(xmlNode, "PageStep", 4));
	abstractSlider->setSingleStep(getAttribute(xmlNode, "SingleStep", 1));
	abstractSlider->setTracking(getAttribute(xmlNode, "Tracking", false));
	abstractSlider->setValue(getAttribute(xmlNode, "Value", 0));
}

QSlider *Runner::loadSlider(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "slider") return 0;
	QSlider *slider = new QSlider(this);
	loadAbstractSlider(slider, xmlNode);
	slider->setTickInterval(getAttribute(xmlNode, "TickInterval", 0));
	slider->setTickPosition(toTickPosition(xmlNode->Attribute("TickPosition")));
	connectSignals(xmlNode, slider, "sliderReleased ()");
	return slider;
}

QSlider::TickPosition Runner::toTickPosition(QString s){
	s = s.toLower();
	if(s == "qslider::noticks" || s == "noticks" || s == "0") return QSlider::NoTicks;
	else if(s == "qslider::ticksbothsides" || s == "ticksbothsides" || s == "3") return QSlider::TicksBothSides;
	else if(s == "qslider::ticksabove" || s == "ticksabove" || s == "1") return QSlider::TicksAbove;
	else if(s == "qslider::ticksbelow" || s == "ticksbelow" || s == "2") return QSlider::TicksBelow;
	else if(s == "qslider::ticksleft" || s == "ticksleft" || s == "1") return QSlider::TicksLeft;
	else if(s == "qslider::ticksright" || s == "ticksright" || s == "2") return QSlider::TicksRight;
	else return QSlider::NoTicks;
}

RibbonSliderPane *Runner::loadRibbonSliderPane(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "ribbonsliderpane") return 0;
	RibbonSliderPane *sliderPane = new RibbonSliderPane(this);
	loadWidget(sliderPane, xmlNode);
	sliderPane->setSingleStep(getAttribute(xmlNode, "SingleStep", 1));
	sliderPane->setValue(getAttribute(xmlNode, "Value", 0));
	sliderPane->setSliderPosition(getAttribute(xmlNode, "SliderPosition", 0));
	sliderPane->setScrollButtons(getAttribute(xmlNode, "ScrollButtons", true));
	connectSignals(xmlNode, sliderPane, "valueChanged(int )");
	return sliderPane;
}

void Runner::createOptions()
{
	Qtitan::RibbonStyle *ribbonStyle = qobject_cast<Qtitan::RibbonStyle*>(qApp->style());
    Qtitan::RibbonStyle::OptionsStyle styleId = Qtitan::RibbonStyle::OS_OFFICE2007BLUE;
    if (ribbonStyle)
        styleId = ribbonStyle->getOptionStyle();

    QMenu* menu = ribbonBar()->addMenu(tr("Options"));
    QAction* actionStyle = menu->addAction(tr("Style"));

    QMenu* menuStyle = new QMenu(ribbonBar());
    QActionGroup* styleActions = new QActionGroup(this);

    QAction* actionBlue = menuStyle->addAction(tr("Office 2007 Blue"));
    actionBlue->setCheckable(true);
    actionBlue->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007BLUE);
    actionBlue->setObjectName("OS_OFFICE2007BLUE");

    QAction* actionBlack = menuStyle->addAction(tr("Office 2007 Black"));
    actionBlack->setObjectName("OS_OFFICE2007BLACK");
    actionBlack->setCheckable(true);
    actionBlack->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007BLACK);

    QAction* actionSilver = menuStyle->addAction(tr("Office 2007 Silver"));
    actionSilver->setObjectName("OS_OFFICE2007SILVER");
    actionSilver->setCheckable(true);
    actionSilver->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007SILVER);

    QAction* actionAqua = menuStyle->addAction(tr("Office 2007 Aqua"));
    actionAqua->setObjectName("OS_OFFICE2007AQUA");
    actionAqua->setCheckable(true);
    actionAqua->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007AQUA);

    QAction* actionScenic = menuStyle->addAction(tr("Windows 7 Scenic"));
    actionScenic->setObjectName("OS_WINDOWS7SCENIC");
    actionScenic->setCheckable(true);
    actionScenic->setChecked(styleId == Qtitan::RibbonStyle::OS_WINDOWS7SCENIC);

    QAction* action2010Blue = menuStyle->addAction(tr("Office 2010 Blue"));
    action2010Blue->setObjectName("OS_OFFICE2010BLUE");
    action2010Blue->setCheckable(true);
    action2010Blue->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2010BLUE);

    QAction* action2010Silver = menuStyle->addAction(tr("Office 2010 Silver"));
    action2010Silver->setObjectName("OS_OFFICE2010SILVER");
    action2010Silver->setCheckable(true);
    action2010Silver->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2010SILVER);

    QAction* action2010Black = menuStyle->addAction(tr("Office 2010 Black"));
    action2010Black->setObjectName("OS_OFFICE2010BLACK");
    action2010Black->setCheckable(true);
    action2010Black->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2010BLACK);

    styleActions->addAction(actionBlue);
    styleActions->addAction(actionBlack);
    styleActions->addAction(actionSilver);
    styleActions->addAction(actionAqua);
    styleActions->addAction(actionScenic);
    styleActions->addAction(action2010Blue);
    styleActions->addAction(action2010Silver);
    styleActions->addAction(action2010Black);

    actionStyle->setMenu(menuStyle);
    connect(styleActions, SIGNAL(triggered(QAction*)), this, SLOT(options(QAction*)));


    QAction* actionMenu = menu->addAction(tr("Font"));

    QMenu* menuFont = new QMenu(ribbonBar());
    QActionGroup* fontActions = new QActionGroup(this);

    m_actionDefault = menuFont->addAction(Font_Default);
    m_actionDefault->setCheckable(true);
    m_actionDefault->setChecked(true);
    m_actionDefault->setObjectName(Font_Default);
    fontActions->addAction(m_actionDefault);

    menuFont->addSeparator();

    QAction* actionNormal = menuFont->addAction(Font_Normal);
    actionNormal->setCheckable(true);
    actionNormal->setObjectName(Font_Normal);
    fontActions->addAction(actionNormal);

    QAction* actionLarge = menuFont->addAction(Font_Large);
    actionLarge->setCheckable(true);
    actionLarge->setObjectName(Font_Large);
    fontActions->addAction(actionLarge);

    QAction* actionExLarge = menuFont->addAction(Font_ExLarge);
    actionExLarge->setCheckable(true);
    actionExLarge->setObjectName(Font_ExLarge);
    fontActions->addAction(actionExLarge);
    actionMenu->setMenu(menuFont);
    connect(fontActions, SIGNAL(triggered(QAction*)), this, SLOT(optionsFont(QAction*)));

    menu->addSeparator();

    QAction* actionCusomize = menu->addAction(tr("Cusomize..."));
    actionCusomize->setEnabled(false);
}

void Runner::options(QAction* action)
{
	Qtitan::RibbonStyle *ribbonStyle = qobject_cast<Qtitan::RibbonStyle*>(qApp->style());
    if (ribbonStyle)
    {
        Qtitan::RibbonStyle::OptionsStyle styleId = Qtitan::RibbonStyle::OS_OFFICE2007BLUE;
        if (action->objectName() == tr("OS_OFFICE2007BLACK"))
            styleId = Qtitan::RibbonStyle::OS_OFFICE2007BLACK;
        else if (action->objectName() == tr("OS_OFFICE2007SILVER"))
            styleId = Qtitan::RibbonStyle::OS_OFFICE2007SILVER;
        else if (action->objectName() == tr("OS_OFFICE2007AQUA"))
            styleId = Qtitan::RibbonStyle::OS_OFFICE2007AQUA;
        else if (action->objectName() == tr("OS_WINDOWS7SCENIC"))
            styleId = Qtitan::OfficeStyle::OS_WINDOWS7SCENIC;
        else if (action->objectName() == tr("OS_OFFICE2010BLUE"))
            styleId = Qtitan::OfficeStyle::OS_OFFICE2010BLUE;
        else if (action->objectName() == tr("OS_OFFICE2010SILVER"))
            styleId = Qtitan::OfficeStyle::OS_OFFICE2010SILVER;
        else if (action->objectName() == tr("OS_OFFICE2010BLACK"))
            styleId = Qtitan::OfficeStyle::OS_OFFICE2010BLACK;

        if (QToolButton* button = ribbonBar()->getSystemButton())
        {
            if (styleId == Qtitan::OfficeStyle::OS_WINDOWS7SCENIC || 
                styleId == Qtitan::OfficeStyle::OS_OFFICE2010BLUE ||
                styleId == Qtitan::OfficeStyle::OS_OFFICE2010SILVER ||
                styleId == Qtitan::OfficeStyle::OS_OFFICE2010BLACK)
                button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            else
                button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        }

        ribbonStyle->setOptionStyle(styleId);
        m_actionDefault->setChecked(true);
    }
}

void Runner::optionsFont(QAction* act)
{
    QFont fnt = ribbonBar()->font();

    if (Font_Default == act->objectName())
        fnt.setPointSize(m_defaultFont);
    else if (Font_Normal == act->objectName())
        fnt.setPointSize(8);
    else if (Font_Large == act->objectName())
        fnt.setPointSize(11);
    else if (Font_ExLarge == act->objectName())
        fnt.setPointSize(13);

    ribbonBar()->setFont(fnt);
}
