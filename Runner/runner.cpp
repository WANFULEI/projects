#include "runner.h"
#include "myLog.h"
#include "commonFunctions.h"
#include <QStackedWidget>
#include <QMessageBox>

Runner::Runner(QWidget *parent, Qt::WFlags flags)
	: RibbonMainWindow(parent, flags)
{
	setObjectName("me");
	MyLog::Init("log");

	TiXmlDocument doc;
	if(doc.LoadFile("config.xml")){
		loadUIFromXml(doc.FirstChildElement("Ribbon"));
	}else{
		LOG_ERROR << "配置文件config.xml不存在或格式不正确！";
	}
}

Runner::~Runner()
{

}

void Runner::loadUIFromXml(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return;
	if(xmlNode->Value() != QString("Ribbon")) return;
	TiXmlElement *pageNode = xmlNode->FirstChildElement("Page");
	while(pageNode){
		if(!isUse(pageNode)){
			pageNode = pageNode->NextSiblingElement("Page");
			continue;
		}
		RibbonBar *pRibbonBar = ribbonBar();
		RibbonPage *ribbonPage = ribbonBar()->addPage(pageNode->Attribute("Name"));
		TiXmlElement *groupNode = pageNode->FirstChildElement("Group");
		while(groupNode){
			if(!isUse(groupNode)){
			groupNode = groupNode->NextSiblingElement("Page");
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
		pageNode = pageNode->NextSiblingElement("Page");
	}
}

QSize Runner::sizeHint() const
{
	return QSize(800, 600);
}

bool Runner::isUse(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return false;
	// 没有配置，默认使用
	if(xmlNode->Attribute("Use") == 0) return true;
	bool use = true;
	xmlNode->QueryBoolAttribute("Use", &use);
	return use;
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
			
		}else if(type == "separator"){
			group->addSeparator();
		}else if(type == "radiobutton"){
			QRadioButton *radioButton = loadRadioButton(itemNode);
			if(radioButton) group->addWidget(QIcon(itemNode->Attribute("Icon")), itemNode->Attribute("Text"), getAttribute(itemNode,
							"Align", true), radioButton);
		}else if(type == "combobox"){
		
		}else if(type == "lineedit"){
			
		}
		itemNode = itemNode->NextSiblingElement("Item");
	}
}

QAction *Runner::loadAction(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "action") return 0;
	QAction *action = new QAction(0);
	action->setObjectName(xmlNode->Attribute("ObjectName"));
	action->setIcon(QIcon(xmlNode->Attribute("Icon")));
	action->setText(xmlNode->Attribute("Text"));
	action->setToolTip(xmlNode->Attribute("ToopTip"));
	action->setVisible(getAttribute(xmlNode, "Visible", true));
	action->setStatusTip(xmlNode->Attribute("StatusTip"));
	if(getAttribute(xmlNode, "Checkable", false)){
		action->setCheckable(true);
		action->setChecked(getAttribute(xmlNode, "Checked", false));
	}
	action->setEnabled(getAttribute(xmlNode, "Enabled", true));
	QString fontName = xmlNode->Attribute("FontFamily");
	int fontSize = getAttribute(xmlNode, "FontPointSize", 9);
	int fontWeight = getAttribute(xmlNode, "FontWeight", -1);
	bool fontItalic = getAttribute(xmlNode, "FontItalic", false);
	if(!fontName.isEmpty()){
		action->setFont(QFont(fontName, fontSize, fontWeight, fontItalic));
	}
	action->setIconText(xmlNode->Attribute("IconText"));
	action->setIconVisibleInMenu(getAttribute(xmlNode, "IconVisibleInMenu", false));
	action->setPriority(toActionPriority(xmlNode->Attribute("Priority")));
	action->setShortcut(QKeySequence(xmlNode->Attribute("Shortcut")));
	action->setShortcutContext(toShortcutContext(xmlNode->Attribute("ShortcutContext")));
	action->setWhatsThis(xmlNode->Attribute("WhatsThis"));
	action->setAutoRepeat(getAttribute(xmlNode, "AutoRepeat", false));
	connectSignals(xmlNode, action, "triggered()");
	return action;
}

QAction::Priority Runner::toActionPriority(QString s){
	s = s.toLower();
	if(s == "qaction::lowpriority" || s == "lowpriority" || s == "0") return QAction::LowPriority;
	else if(s == "qaction::normalpriority" || s == "normalpriority" || s == "128") return QAction::NormalPriority;
	else if(s == "qaction::highpriority" || s == "highpriority" || s == "256") return QAction::HighPriority;
	else return QAction::NormalPriority;
}

Qt::ShortcutContext Runner::toShortcutContext(QString s){
	s = s.toLower();
	if(s == "qt::widgetshortcut" || s == "widgetshortcut" || s == "0") return Qt::WidgetShortcut;
	else if(s == "qt::widgetwithchildrenshortcut" || s == "widgetwithchildrenshortcut" || s == "3") return Qt::WidgetWithChildrenShortcut;
	else if(s == "qt::windowshortcut" || s == "windowshortcut" || s == "1") return Qt::WindowShortcut;
	else if(s == "qt::applicationshortcut" || s == "applicationshortcut" || s == "2") return Qt::ApplicationShortcut;
	else return Qt::WindowShortcut;
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
	QMenu *menu = new QMenu(0);
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
	widget->setAutoFillBackground(getAttribute(xmlNode, "AutoFillBackground", false));
	int width = getAttribute(xmlNode, "Width", 0);
	int height = getAttribute(xmlNode, "Height", 0);
	if(width > 0 && height > 0) widget->resize(width, height);
	widget->setCursor(toCursorShape(xmlNode->Attribute("Cursor")));
	widget->setEnabled(getAttribute(xmlNode, "Enabled", true));
	widget->setFocusPolicy(toFocusPolicy(xmlNode->Attribute("FocusPolicy")));
	QString fontName = xmlNode->Attribute("FontFamily");
	int fontSize = getAttribute(xmlNode, "FontPointSize", 9);
	int fontWeight = getAttribute(xmlNode, "FontWeight", -1);
	bool fontItalic = getAttribute(xmlNode, "FontItalic", false);
	if(!fontName.isEmpty()){
		widget->setFont(QFont(fontName, fontSize, fontWeight, fontItalic));
	}
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
	widget->setVisible(getAttribute(xmlNode, "Visible", true));
	widget->setWhatsThis(xmlNode->Attribute("WhatsThis"));
	widget->setWindowIcon(QIcon(xmlNode->Attribute("WindowIcon")));
	widget->setWindowIconText(xmlNode->Attribute("WindowIconText"));
	widget->setWindowOpacity(getAttribute(xmlNode, "WindowOpacity", 1.0));
	widget->setWindowTitle(xmlNode->Attribute("WindowTitle"));
}

Qt::CursorShape Runner::toCursorShape(QString s){
	s = s.toLower();
	if(s == "qt::arrowcursor" || s == "arrowcursor" || s == "0") return Qt::ArrowCursor;
	else if(s == "qt::uparrowcursor" || s == "uparrowcursor" || s == "1") return Qt::UpArrowCursor;
	else if(s == "qt::crosscursor" || s == "crosscursor" || s == "2") return Qt::CrossCursor;
	else if(s == "qt::waitcursor" || s == "waitcursor" || s == "3") return Qt::WaitCursor;
	else if(s == "qt::ibeamcursor" || s == "ibeamcursor" || s == "4") return Qt::IBeamCursor;
	else if(s == "qt::sizevercursor" || s == "sizevercursor" || s == "5") return Qt::SizeVerCursor;
	else if(s == "qt::sizehorcursor" || s == "sizehorcursor" || s == "6") return Qt::SizeHorCursor;
	else if(s == "qt::sizebdiagcursor" || s == "sizebdiagcursor" || s == "7") return Qt::SizeBDiagCursor;
	else if(s == "qt::sizefdiagcursor" || s == "sizefdiagcursor" || s == "8") return Qt::SizeFDiagCursor;
	else if(s == "qt::sizeallcursor" || s == "sizeallcursor" || s == "9") return Qt::SizeAllCursor;
	else if(s == "qt::blankcursor" || s == "blankcursor" || s == "10") return Qt::BlankCursor;
	else if(s == "qt::splitvcursor" || s == "splitvcursor" || s == "11") return Qt::SplitVCursor;
	else if(s == "qt::splithcursor" || s == "splithcursor" || s == "12") return Qt::SplitHCursor;
	else if(s == "qt::pointinghandcursor" || s == "pointinghandcursor" || s == "13") return Qt::PointingHandCursor;
	else if(s == "qt::forbiddencursor" || s == "forbiddencursor" || s == "14") return Qt::ForbiddenCursor;
	else if(s == "qt::openhandcursor" || s == "openhandcursor" || s == "17") return Qt::OpenHandCursor;
	else if(s == "qt::closedhandcursor" || s == "closedhandcursor" || s == "18") return Qt::ClosedHandCursor;
	else if(s == "qt::whatsthiscursor" || s == "whatsthiscursor" || s == "15") return Qt::WhatsThisCursor;
	else if(s == "qt::busycursor" || s == "busycursor" || s == "16") return Qt::BusyCursor;
	else if(s == "qt::dragmovecursor" || s == "dragmovecursor" || s == "20") return Qt::DragMoveCursor;
	else if(s == "qt::dragcopycursor" || s == "dragcopycursor" || s == "19") return Qt::DragCopyCursor;
	else if(s == "qt::draglinkcursor" || s == "draglinkcursor" || s == "21") return Qt::DragLinkCursor;
	else if(s == "qt::bitmapcursor" || s == "bitmapcursor" || s == "24") return Qt::BitmapCursor;
	else return Qt::ArrowCursor;
}

Qt::FocusPolicy Runner::toFocusPolicy(QString s){
	s = s.toLower();
	if(s == "qt::tabfocus" || s == "tabfocus" || s == "1") return Qt::TabFocus;
	else if(s == "qt::clickfocus" || s == "clickfocus" || s == "2") return Qt::ClickFocus;
	else if(s == "qt::strongfocus" || s == "strongfocus" || s == "11") return Qt::StrongFocus;
	else if(s == "qt::wheelfocus" || s == "wheelfocus" || s == "15") return Qt::WheelFocus;
	else if(s == "qt::nofocus" || s == "nofocus" || s == "0") return Qt::NoFocus;
	else return Qt::NoFocus;
}

QCheckBox *Runner::loadCheckBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "checkbox") return 0;
	QCheckBox *checkBox = new QCheckBox;
	loadWidget(checkBox, xmlNode);
	loadAbstractButton(checkBox, xmlNode);
	checkBox->setCheckState(toCheckState(xmlNode->Attribute("CheckState")));
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
	button->setAutoExclusive(getAttribute(xmlNode, "AutoExclusize", false));
	button->setAutoRepeat(getAttribute(xmlNode, "AutoRepeat", false));
	button->setAutoRepeatDelay(getAttribute(xmlNode, "AutoRepeatDelay", 200));
	button->setAutoRepeatInterval(getAttribute(xmlNode, "AutoRepeatInterval", 200));
	button->setCheckable(getAttribute(xmlNode, "Checkable", true));
	button->setChecked(getAttribute(xmlNode, "Checked", false));
	button->setIcon(QIcon(xmlNode->Attribute("Icon")));
	int width = getAttribute(xmlNode, "IconWidth", 0);
	int height = getAttribute(xmlNode, "IconHeight", 0);
	if(width > 0 && height > 0) button->setIconSize(QSize(width, height));
	button->setText(xmlNode->Attribute("Text"));
}

QRadioButton *Runner::loadRadioButton(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type = xmlNode->Attribute("Type");
	if(type.toLower() != "radiobutton") return 0;
	QRadioButton *radioButton = new QRadioButton;
	loadWidget(radioButton, xmlNode);
	loadAbstractButton(radioButton, xmlNode);
	radioButton->setAutoExclusive(getAttribute(xmlNode, "AutoExclusize", true));
	return radioButton;
}