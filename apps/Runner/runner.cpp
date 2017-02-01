#include "runner.h"
#include "component/Log.h"
#include "commonFunctions.h"
#include <QStackedWidget>
#include <QMessageBox>
#include <QApplication>
#include <qdesktopwidget.h>
#include <QLibrary>
#include "component/globalInstance.h"
#include "QDir"
#include "QPluginLoader"

#define Prop_ContextColor   "ContextColor"

#define Font_Default  "System Default"
#define Font_Normal   "Normal"
#define Font_Large    "Large"
#define Font_ExLarge  "Extra Large"


Runner::Runner(QWidget *parent, Qt::WFlags flags)
	: RibbonMainWindow(parent, flags)
{
	QDir::setCurrent(qApp->applicationDirPath());

	global->setMainWindow(this);
	m_defaultFont = 8;
	m_actionDefault = 0;
	setObjectName("runner");
	Log::init("log");

	TiXmlDocument doc;
	if(doc.LoadFile("..\\config\\runnerConfig.xml")){
		TiXmlElement *rootNode = doc.RootElement();
		loadComponents(rootNode->FirstChildElement("Components"));
		loadUIFromXml(rootNode->FirstChildElement("Ribbon"));
		auto iter = m_components.begin();
		while(iter != m_components.end()){
			Component *com = dynamic_cast<Component *>(*iter);
			if(com){
				com->initialize();
			}
			++iter;
		}
	}else{
		LOG_ERROR << "配置文件config.xml不存在或格式不正确！";
	}

	ribbonBar()->setFrameThemeEnabled(true);
	m_defaultFont = ribbonBar()->font().pointSize();
}

void Runner::test(TiXmlElement *xmlNode){
	if(xmlNode == 0) return;
	TiXmlAttribute *attr = xmlNode->FirstAttribute();
	while(attr){
		TiXmlElement *tmp = (TiXmlElement *)xmlNode->InsertEndChild(TiXmlElement(attr->Name()));
		tmp->InsertEndChild(TiXmlText(attr->Value()));
		TiXmlAttribute *next = attr->Next();
		xmlNode->RemoveAttribute(attr->Name());
		attr = next;
	}
	TiXmlElement *ele = xmlNode->FirstChildElement();
	while(ele){
		test(ele);
		ele = ele->NextSiblingElement();
	}
}

Runner::~Runner()
{
	
}

void Runner::loadUIFromXml(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return;
	if(xmlNode->Value() != QString("Ribbon")) return;

	TiXmlElement *childNode = xmlNode->FirstChildElement();
	while(childNode){
		if(!isUse(childNode)){
			childNode = childNode->NextSiblingElement();
			continue;
		}
		if(childNode->Value() == QString("Page")){
			loadPage(childNode);
		}else if(childNode->Value() == QString("RightAction")){
			loadRightAction(childNode);
		}else if(childNode->Value() == QString("QuickAccess")){
			loadQuickAccess(childNode);
		}else if(childNode->Value() == QString("StatusBar")){
			loadStatusBar(childNode);
		}else if(childNode->Value() == QString("MainButton")){
			loadMainButton(childNode);
		}else if(childNode->Value() == QString("ToolBar")){
			loadToolBar(childNode);
		}else if(childNode->Value() == QString("DockWidget")){
			loadDockWidget(childNode);
		}else if(childNode->Value() == QString("TabCenterWidget")){
			loadTabCenterWidget(childNode);
		}
		childNode = childNode->NextSiblingElement();
	}
}

void Runner::loadTabCenterWidget(TiXmlElement *xmlNode){
	if(xmlNode == 0) return;
	if(!isUse(xmlNode)) return;
	QTabWidget *tab= new QTabWidget(this);
	loadWidget(tab, xmlNode);
	TiXmlElement *itemNode = xmlNode->FirstChildElement("Item");
	while(itemNode){
		QObject *item = loadItem(itemNode);
		if(dynamic_cast<QWidget *>(item)){
			QString icon, text;
			getElementText(itemNode, "Icon", icon);
			getElementText(itemNode, "Label", text);
			tab->addTab(dynamic_cast<QWidget *>(item), QIcon(icon), text);
		}
		itemNode = itemNode->NextSiblingElement("Item");
	}
	int i, width, height;
	bool b;
	QString s;
	if(getElementInt(xmlNode, "CurrentIndex", i)) tab->setCurrentIndex(i);
	if(getElementInt(xmlNode, "IconWidth", width) && getElementInt(xmlNode, "IconHeight", height)) tab->setIconSize(QSize(width, height));
	if(getElementBool(xmlNode, "DocumentMode", b)) tab->setDocumentMode(b);
	if(getElementBool(xmlNode, "Movable", b)) tab->setMovable(b);
	if(getElementBool(xmlNode, "TabsClosable", b)) tab->setTabsClosable(b);
	if(getElementBool(xmlNode, "UsesScrollButtons", b)) tab->setUsesScrollButtons(b);
	if(getElementText(xmlNode, "TabPosition", s)) tab->setTabPosition(toTabPosition(s));
	if(getElementText(xmlNode, "TabShape", s)) tab->setTabShape(toTabShape(s));
	setCentralWidget(tab);
}

QTabWidget::TabPosition Runner::toTabPosition(QString s){
	s = s.toLower();
	if(s == "qtabwidget::north" || s == "north" || s == "0") return QTabWidget::North;
	else if(s == "qtabwidget::south" || s == "south" || s == "1") return QTabWidget::South;
	else if(s == "qtabwidget::west" || s == "west" || s == "2") return QTabWidget::West;
	else if(s == "qtabwidget::east" || s == "east" || s == "3") return QTabWidget::East;
	else return QTabWidget::North;
}

QTabWidget::TabShape Runner::toTabShape(QString s){
	s = s.toLower();
	if(s == "qtabwidget::rounded" || s == "rounded" || s == "0") return QTabWidget::Rounded;
	else if(s == "qtabwidget::triangular" || s == "triangular" || s == "1") return QTabWidget::Triangular;
	else return QTabWidget::Rounded;
}

void Runner::loadToolBar(TiXmlElement *xmlNode){
	if(xmlNode == 0) return;
	if(!isUse(xmlNode)) return;
	QToolBar *toolbar = new QToolBar(this);
	connectSignals(xmlNode, toolbar, "actionTriggered ( QAction * )");
	loadToolBar(toolbar, xmlNode);
	QString s;
	getElementText(xmlNode, "ToolBarArea", s);
	addToolBar(toToolBarArea(s), toolbar);
}

void Runner::loadToolBar(QToolBar *toolbar, TiXmlElement *xmlNode){
	if(toolbar == 0 || xmlNode == 0) return;
	loadWidget(toolbar, xmlNode);
	int i, width, height;
	bool b;
	double d;
	QString s;
	if(getElementText(xmlNode, "AllowedAreas", s)) toolbar->setAllowedAreas(toToolBarAreas(s));
	if(getElementText(xmlNode, "Orientation", s)) toolbar->setOrientation(toOrientation(s));
	if(getElementText(xmlNode, "ToolButtonStyle", s)) toolbar->setToolButtonStyle(toToolButtonStyle(s));
	if(getElementText(xmlNode, "WindowIcon", s)) toolbar->setWindowIcon(QIcon(s));
	if(getElementText(xmlNode, "WindowIconText", s)) toolbar->setWindowIconText(s);
	if(getElementText(xmlNode, "WindowTitle", s)) toolbar->setWindowTitle(s);
	if(getElementBool(xmlNode, "Floatable", b)) toolbar->setFloatable(b);
	if(getElementBool(xmlNode, "Movable", b)) toolbar->setMovable(b);
	if(getElementDouble(xmlNode, "WindowOpacity", d)) toolbar->setWindowOpacity(d);
	if(getElementInt(xmlNode, "IconWidth", width) && getElementInt(xmlNode, "IconHeight", height)) toolbar->setIconSize(QSize(width, height));
	TiXmlElement *itemNode = xmlNode->FirstChildElement("Item");
	while(itemNode){
		QString type;
		getElementText(itemNode, "Type", type);
		if(type == QString("separator")){
			toolbar->addSeparator();
			itemNode = itemNode->NextSiblingElement("Item");
			continue;
		}
		QObject *item = loadItem(itemNode);
		if(dynamic_cast<QAction *>(item)){
			toolbar->addAction(dynamic_cast<QAction *>(item));
		}else if(dynamic_cast<QWidget *>(item)){
			toolbar->addWidget(dynamic_cast<QWidget *>(item));
		}
		itemNode = itemNode->NextSiblingElement("Item");
	}
}

Qt::ToolBarAreas Runner::toToolBarAreas(QString s){
	s = s.toLower();
	if(s == "qt::lefttoolbararea" || s == "lefttoolbararea" || s == "1") return Qt::LeftToolBarArea;
	else if(s == "qt::righttoolbararea" || s == "righttoolbararea" || s == "2") return Qt::RightToolBarArea;
	else if(s == "qt::toptoolbararea" || s == "toptoolbararea" || s == "4") return Qt::TopToolBarArea;
	else if(s == "qt::bottomtoolbararea" || s == "bottomtoolbararea" || s == "8") return Qt::BottomToolBarArea;
	else if(s == "qt::alltoolbarareas" || s == "alltoolbarareas" || s == "15") return Qt::AllToolBarAreas;
	else {
		int areas = s.toInt();
		if(areas == 0) return Qt::AllToolBarAreas;
		else return Qt::ToolBarAreas(areas);
	}
}

Qt::ToolBarArea Runner::toToolBarArea(QString s){
	s = s.toLower();
	if(s == "qt::lefttoolbararea" || s == "lefttoolbararea" || s == "1") return Qt::LeftToolBarArea;
	else if(s == "qt::righttoolbararea" || s == "righttoolbararea" || s == "2") return Qt::RightToolBarArea;
	else if(s == "qt::toptoolbararea" || s == "toptoolbararea" || s == "4") return Qt::TopToolBarArea;
	else if(s == "qt::bottomtoolbararea" || s == "bottomtoolbararea" || s == "8") return Qt::BottomToolBarArea;
//	else if(s == "qt::alltoolbarareas" || s == "alltoolbarareas" || s == "15") return Qt::AllToolBarAreas;
	else return Qt::LeftToolBarArea;
}

void Runner::loadPage(TiXmlElement *xmlNode){
	QString s;
	getElementText(xmlNode, "Name", s);
	RibbonPage *ribbonPage = ribbonBar()->addPage(s);
	TiXmlElement *groupNode = xmlNode->FirstChildElement("Group");
	while(groupNode){
		if(!isUse(groupNode)){
			groupNode = groupNode->NextSiblingElement("Group");
			continue;
		}
		getElementText(groupNode, "Name", s);
		RibbonGroup *ribbonGroup = ribbonPage->addGroup(s);
		bool b;
		if(getElementBool(groupNode, "OptionButtonVisible", b) && b){
			ribbonGroup->setOptionButtonVisible(true);
			QAction *act = ribbonGroup->getOptionButtonAction();
			if(getElementText(groupNode, "ToolTip", s)) act->setToolTip(s);
			connectSignals(groupNode, act, "triggered()");
		}
		if(getElementBool(groupNode, "ControlsCentering", b)) ribbonGroup->setControlsCentering(b);
		if(getElementBool(groupNode, "ControlsGrouping", b)) ribbonGroup->setControlsGrouping(b);
		loadItems(groupNode, ribbonGroup);
		groupNode = groupNode->NextSiblingElement("Group");
	}
}

void Runner::loadDockWidget(TiXmlElement *xmlNode){
	if(xmlNode == 0) return;
	if(!isUse(xmlNode)) return;
	QDockWidget *dock = new QDockWidget(this);
	loadWidget(dock, xmlNode);
	int i;
	bool b;
	QString s;
	if(getElementText(xmlNode, "WindowIcon", s)) dock->setWindowIcon(QIcon(s));
	if(getElementText(xmlNode, "WindowIconText", s)) dock->setWindowIconText(s);
	if(getElementInt(xmlNode, "WindowOpacity", i)) dock->setWindowOpacity(i);
	if(getElementText(xmlNode, "WindowTitle", s)) dock->setWindowTitle(s);
	if(getElementText(xmlNode, "AllowedAreas", s)) dock->setAllowedAreas(toDockWidgetAreas(s));
	if(getElementText(xmlNode, "Features", s)) dock->setFeatures(toDockWidgetFeatures(s));
	if(getElementBool(xmlNode, "Floating", b)) dock->setFloating(b);
	connectSignals(xmlNode, dock, "visibilityChanged ( bool )");
	if(getElementText(xmlNode, "DockArea", s)) addDockWidget((Qt::DockWidgetArea)(int)toDockWidgetAreas(s), dock);
	else addDockWidget(Qt::LeftDockWidgetArea, dock);
}


QDockWidget::DockWidgetFeatures Runner::toDockWidgetFeatures(QString s){
	s = s.toLower();
	if(s == "qt::dockwidgetclosable" || s == "dockwidgetclosable" || s == "1") return QDockWidget::DockWidgetClosable;
	else if(s == "qt::dockwidgetmovable" || s == "dockwidgetmovable" || s == "2") return QDockWidget::DockWidgetMovable;
	else if(s == "qt::dockwidgetfloatable" || s == "dockwidgetfloatable" || s == "4") return QDockWidget::DockWidgetFloatable;
	else if(s == "qt::dockwidgetverticaltitlebar" || s == "dockwidgetverticaltitlebar" || s == "8") return QDockWidget::DockWidgetVerticalTitleBar;
	else if(s == "qt::alldockwidgetfeatures" || s == "alldockwidgetfeatures" || s == "7") return QDockWidget::AllDockWidgetFeatures;
	else {
		int areas = s.toInt();
		if(areas == 0) return QDockWidget::AllDockWidgetFeatures;
		else return QDockWidget::DockWidgetFeatures(areas);
	}
}

Qt::DockWidgetAreas Runner::toDockWidgetAreas(QString s){
	s = s.toLower();
	if(s == "qt::leftdockwidgetarea" || s == "leftdockwidgetarea" || s == "1") return Qt::LeftDockWidgetArea;
	else if(s == "qt::rightdockwidgetarea" || s == "rightdockwidgetarea" || s == "2") return Qt::RightDockWidgetArea;
	else if(s == "qt::topdockwidgetarea" || s == "topdockwidgetarea" || s == "4") return Qt::TopDockWidgetArea;
	else if(s == "qt::bottomdockwidgetarea" || s == "bottomdockwidgetarea" || s == "8") return Qt::BottomDockWidgetArea;
	else if(s == "qt::alldockwidgetareas" || s == "alldockwidgetareas" || s == "15") return Qt::AllDockWidgetAreas;
	else {
		int areas = s.toInt();
		if(areas == 0) return Qt::AllDockWidgetAreas;
		else return Qt::DockWidgetAreas(areas);
	}
}

void Runner::loadRightAction(TiXmlElement *xmlNode){
	TiXmlElement *actionNode = xmlNode->FirstChildElement("Item");
	while(actionNode){
		QString type;
		getElementText(actionNode, "Type", type);
		if(type.toLower() == "action"){
			QString icon, text, buttonStyle;
			getElementText(actionNode, "Icon", icon);
			getElementText(actionNode, "Text", text);
			getElementText(actionNode, "ToolButtonStyle", buttonStyle);
			QAction *action = ribbonBar()->addAction(QIcon(icon), text, toToolButtonStyle(buttonStyle));
			connectSignals(actionNode, action, "triggered()");
		}else if(type.toLower() == "menu"){
			QMenu *menu = loadMenu(actionNode);
			QString icon, text, buttonStyle;
			getElementText(actionNode, "Icon", icon);
			getElementText(actionNode, "Text", text);
			getElementText(actionNode, "ToolButtonStyle", buttonStyle);
			QAction *action = ribbonBar()->addAction(QIcon(icon), text, toToolButtonStyle(buttonStyle), menu);
		}else if(type.toLower() == "options"){
			createOptions();
		}
		actionNode = actionNode->NextSiblingElement("Item");
	}
}

void Runner::loadQuickAccess(TiXmlElement *xmlNode){
	RibbonQuickAccessBar* quickAccessBar = ribbonBar()->getQuickAccessBar();
	QAction* action = quickAccessBar->actionCustomizeButton();
	action->setToolTip(tr("Customize Quick Access Bar"));
	TiXmlElement *actionNode = xmlNode->FirstChildElement("Item");
	while(actionNode){
		QAction *action = loadAction(actionNode);
		if(action) quickAccessBar->addAction(action);
		actionNode = actionNode->NextSiblingElement("Item");
	}
	ribbonBar()->showQuickAccess(true);
}

void Runner::loadStatusBar(TiXmlElement *xmlNode){
	TiXmlElement *itemNode = xmlNode->FirstChildElement("Item");
	while(itemNode){
		QObject *item = loadItem(itemNode);
		if(dynamic_cast<QAction *>(item)){
			statusBar()->addAction(dynamic_cast<QAction *>(item));
		}else if(dynamic_cast<QWidget *>(item)){
			int i;
			bool b;
			getElementInt(itemNode, "Stretch", i);
			if(getElementBool(itemNode, "PermanentWidget", b) && b){
				statusBar()->addPermanentWidget(dynamic_cast<QWidget *>(item), i);
			}else{
				statusBar()->addWidget(dynamic_cast<QWidget *>(item), i);
			}
		}
		itemNode = itemNode->NextSiblingElement("Item");
	}
}

void Runner::loadMainButton(TiXmlElement *xmlNode){
	QString icon, text;
	getElementText(xmlNode, "Logo", icon);
	getElementText(xmlNode, "Text", text);
	QAction *actionFile = ribbonBar()->addSystemButton(QIcon(icon), text);
	Qtitan::RibbonSystemPopupBar* popupBar = qobject_cast<Qtitan::RibbonSystemPopupBar*>(actionFile->menu());
	TiXmlElement *actionNode = xmlNode->FirstChildElement("Item");
	while(actionNode){
		QString type;
		getElementText(actionNode, "Type", type);
		if(type.toLower() == "action"){
			QAction *action = loadAction(actionNode);
			if(action){
				bool b;
				QString s;
				getElementText(actionNode, "ToolButtonStyle", s);
				if(getElementBool(actionNode, "PopupBarAction", b) && b){
					popupBar->addPopupBarAction(action, toToolButtonStyle(s));
				}else{
					popupBar->addAction(action);
				}
			}
		}else if(type.toLower() == "menu"){
			QMenu *menu = loadMenu(actionNode);
			popupBar->addMenu(menu);
		}
		actionNode = actionNode->NextSiblingElement("Item");
	}
	QString s = tr("Recent Files");
	int i = 10;
	getElementText(xmlNode, "PageRecentFile", s);
	getElementInt(xmlNode, "FileCount", i);
	RibbonPageSystemRecentFileList* pageRecentFile = popupBar->addPageRecentFile(s);
	pageRecentFile->setSize(i);
}

QSize Runner::sizeHint() const
{
	return QSize(800, 600);
}

bool Runner::isUse(TiXmlElement *xmlNode)
{
	if(xmlNode == 0) return false;
	bool b = true;
	return !getElementBool(xmlNode, "Use", b) || b;
}

void Runner::connectSignals(TiXmlElement *xmlNode, QObject *sender, QString defaultSignal){
	if(xmlNode == 0 || sender == 0) return;
	TiXmlElement *connectionNode = xmlNode->FirstChildElement("Connection");
	while(connectionNode){
		if(!isUse(connectionNode)){
			connectionNode = connectionNode->NextSiblingElement("Connection");	
			continue;
		}
		QString receiverName;
		getElementText(connectionNode, "Receiver", receiverName);
		if(receiverName.isEmpty()){
			LOG_INFO << "row=" << QString::number(connectionNode->Row()).toStdString() << ",receiver is empty";
			connectionNode = connectionNode->NextSiblingElement("Connection");
 			continue;
		}
		QString signalName = defaultSignal;
		getElementText(connectionNode, "Signal", signalName);
		if(signalName.isEmpty()){
			LOG_INFO << "row=" << QString::number(connectionNode->Row()).toStdString().c_str() << ",signal is empty";
			connectionNode = connectionNode->NextSiblingElement("Connection");	
			continue;
		}
		QString slotName;
		getElementText(connectionNode, "Slot", slotName);
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

void Runner::loadItems(TiXmlElement *xmlNode, RibbonGroup *group, QActionGroup *actionGroup){
	if(xmlNode == 0 || group == 0) return;
	TiXmlElement *itemNode = xmlNode->FirstChildElement("Item");
	while(itemNode){
		QString type;
		getElementText(itemNode, "Type", type);
		if(type.toLower() == QString("separator")){
			group->addSeparator();
			itemNode = itemNode->NextSiblingElement("Item");
			continue;
		}
		QObject *item = loadItem(itemNode);
		if(dynamic_cast<QAction *>(item)){
			QString s;
			getElementText(itemNode, "ToolButtonStyle", s);
			group->addAction(dynamic_cast<QAction *>(item), toToolButtonStyle(s));
			if(actionGroup) actionGroup->addAction(dynamic_cast<QAction *>(item));
		}else if(dynamic_cast<QMenu *>(item)){
			QString icon, text, buttonStyle;
			getElementText(itemNode, "Icon", icon);
			getElementText(itemNode, "Text", text);
			getElementText(itemNode, "ToolButtonStyle", buttonStyle);
			group->addAction(QIcon(icon), text, 
				toToolButtonStyle(buttonStyle), dynamic_cast<QMenu *>(item));
		}else if(dynamic_cast<QWidget *>(item)){
			QString icon, text;
			bool align = true;
			getElementText(itemNode, "Icon", icon);
			getElementText(itemNode, "Text", text);
			getElementBool(itemNode, "Align", align);
			group->addWidget(QIcon(icon), text, align, dynamic_cast<QWidget *>(item));
		}else if(dynamic_cast<QActionGroup *>(item)){
			QActionGroup *actionGroup = dynamic_cast<QActionGroup *>(item);
			loadItems(itemNode, group, actionGroup);
		}
		itemNode = itemNode->NextSiblingElement("Item");
	}
}

QObject *Runner::loadItem(TiXmlElement *itemNode){
	QString type;
	getElementText(itemNode, "Type", type);
	type = type.toLower();
	if(type == "action"){
		return loadAction(itemNode);
	}else if(type == "menu"){
		return loadMenu(itemNode);
	}else if(type == "checkbox"){
		return loadCheckBox(itemNode);
	}else if(type == "spinbox"){
		return loadSpinBox(itemNode);
	}else if(type == "radiobutton"){
		return loadRadioButton(itemNode);
	}else if(type == "combobox"){
		return loadComboBox(itemNode);
	}else if(type == "lineedit"){
		return loadLineEdit(itemNode);
	}else if(type == "fontcombobox"){
		return loadFontComboBox(itemNode);
	}else if(type == "label"){
		return loadLable(itemNode);
	}else if(type == "progressbar"){
		return loadProgressBar(itemNode);
	}else if(type == "scrollbar"){
		return loadScrollBar(itemNode);
	}else if(type == "slider"){
		return loadSlider(itemNode);
	}else if(type == "ribbonsliderpane"){
		return loadRibbonSliderPane(itemNode);
	}else if(type == "widget"){
		QWidget *widget = new QWidget(this);
		loadWidget(widget, itemNode);
		return widget;
	}else if(type == "actiongroup"){
		QActionGroup *group = new QActionGroup(this);
		bool exclusive = true;
		getElementBool(itemNode, "Exclusive", exclusive);
		group->setExclusive(exclusive);
		QString s;
		if(getElementText(itemNode, "ObjectName", s)) group->setObjectName(s);
		return group;
	}else{
		return 0;
	}
}

QAction *Runner::loadAction(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	if(!getElementText(xmlNode, "Type", type)) return 0;
	if(type.toLower() != "action") return 0;
	QAction *action = new QAction(this);
	QString s;
	bool b;
	if(getElementText(xmlNode, "ObjectName", s)) action->setObjectName(s);
	if(getElementText(xmlNode, "Icon", s)) action->setIcon(QIcon(s));
	if(getElementText(xmlNode, "Text", s)) action->setText(s);
	if(getElementText(xmlNode, "ToopTip", s)) action->setToolTip(s);
	if(getElementBool(xmlNode, "Visible", b)){
		if(!b) action->setVisible(false);
	}
	if(getElementText(xmlNode, "StatusTip", s)) action->setStatusTip(s);
	if(getElementBool(xmlNode, "Checkable", b)){
		action->setCheckable(b);
		if(getElementBool(xmlNode, "Checked", b)) action->setChecked(b);
	}
	if(getElementBool(xmlNode, "Enabled", b)) action->setEnabled(b);
	if(getElementText(xmlNode, "Shortcut", s)) action->setShortcut(s);
	connectSignals(xmlNode, action, "triggered()");
	return action;
}

Qt::ToolButtonStyle Runner::toToolButtonStyle(QString s){
	s = s.toLower();
	if(s == "qt::toolbuttonicononly" || s == "toolbuttonicononly" || s == "0") return Qt::ToolButtonIconOnly;
	else if(s == "qt::toolbuttontextonly" || s == "toolbuttontextonly" || s == "1") return Qt::ToolButtonTextOnly;
	else if(s == "qt::toolbuttontextbesideicon" || s == "toolbuttontextbesideicon" || s == "2") return Qt::ToolButtonTextBesideIcon;
	else if(s == "qt::toolbuttontextundericon" || s == "toolbuttontextundericon" || s == "3") return Qt::ToolButtonTextUnderIcon;
	else if(s == "qt::toolbuttonfollowstyle" || s == "toolbuttonfollowstyle" || s == "4") return Qt::ToolButtonFollowStyle;
	else return Qt::ToolButtonIconOnly;
}

QMenu *Runner::loadMenu(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	getElementText(xmlNode, "Type", type);
	if(type.toLower() != "menu") return 0;
	QMenu *menu = new QMenu(this);
	loadWidget(menu, xmlNode);
	QString s;
	if(getElementText(xmlNode, "Icon", s)) menu->setIcon(QIcon(s));
	if(getElementText(xmlNode, "Text", s)) menu->setTitle(s);
	TiXmlElement *childNode = xmlNode->FirstChildElement("Item");
	while(childNode){
		QString childType;
		getElementText(childNode, "Type", childType);
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
	QString s;
	int i;
	bool b;
	double d;
	if(getElementText(xmlNode, "ObjectName", s)) widget->setObjectName(s);
	if(getElementText(xmlNode, "StatusTip", s)) widget->setStatusTip(s);
	if(getElementText(xmlNode, "StyleSheet", s)) widget->setStyleSheet(s);
	if(getElementText(xmlNode, "ToolTip", s)) widget->setToolTip(s);
	if(getElementText(xmlNode, "WindowIcon", s)) widget->setWindowIcon(QIcon(s));
	if(getElementText(xmlNode, "WindowIconText", s)) widget->setWindowIconText(s);
	if(getElementText(xmlNode, "WindowTitle", s)) widget->setWindowTitle(s);
	if(getElementInt(xmlNode, "MinimumWidth", i)) widget->setMinimumWidth(i);
	if(getElementInt(xmlNode, "MaximumWidth", i)) widget->setMaximumWidth(i);
	if(getElementInt(xmlNode, "MinimumHeight", i)) widget->setMinimumHeight(i);
	if(getElementInt(xmlNode, "MaximumHeight", i)) widget->setMaximumHeight(i);
	if(getElementBool(xmlNode, "Enabled", b)) widget->setEnabled(b);
	if(getElementBool(xmlNode, "Visible", b) && !b) widget->setVisible(false);
	if(getElementDouble(xmlNode, "WindowOpacity", d)) widget->setWindowOpacity(d);
}

QCheckBox *Runner::loadCheckBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	if(!getElementText(xmlNode, "Type", type)) return 0;
	if(type.toLower() != "checkbox") return 0;
	QCheckBox *checkBox = new QCheckBox(this);
	loadAbstractButton(checkBox, xmlNode);
	int i;
	QString s;
	if(getElementText(xmlNode, "CheckState", s)) checkBox->setCheckState(toCheckState(s));
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
	bool value = true;
	if(getElementBool(xmlNode, "Checkable", value)) button->setCheckable(value);
	if(getElementBool(xmlNode, "Checked", value)) button->setChecked(value);
	QString s;
	if(getElementText(xmlNode, "Icon", s)) button->setIcon(QIcon(s));
	int width, height;
	if(getElementInt(xmlNode, "IconWidth", width) && getElementInt(xmlNode, "IconHeight", height)) button->setIconSize(QSize(width, height));
	if(getElementText(xmlNode, "Text", s)) button->setText(s);
}

QRadioButton *Runner::loadRadioButton(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	getElementText(xmlNode, "Type", type);
	if(type.toLower() != "radiobutton") return 0;
	QRadioButton *radioButton = new QRadioButton(this);
	loadAbstractButton(radioButton, xmlNode);
	connectSignals(xmlNode, radioButton, "clicked ( bool checked )");
	return radioButton;
}

QLineEdit *Runner::loadLineEdit(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	if(!getElementText(xmlNode, "Type", type)) return 0;
	if(type.toLower() != "lineedit") return 0;
	QLineEdit *lineEdit = new QLineEdit(this);
	loadWidget(lineEdit, xmlNode);
	bool b;
	QString s;
	int i;
	if(getElementBool(xmlNode, "ReadOnly", b)) lineEdit->setReadOnly(b);
	if(getElementBool(xmlNode, "Frame", b)) lineEdit->setFrame(b);
	if(getElementInt(xmlNode, "MaxLength", i)) lineEdit->setMaxLength(i);
	if(getElementText(xmlNode, "Alignment", s)) lineEdit->setAlignment(toAlignment(s));
	if(getElementText(xmlNode, "InputMask", s)) lineEdit->setInputMask(s);
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
	QString type;
	if(!getElementText(xmlNode, "Type", type)) return 0;
	if(type.toLower() != "combobox") return 0;
	QComboBox *comboBox = new QComboBox(this);
	loadWidget(comboBox, xmlNode);
	TiXmlElement *itemNode = xmlNode->FirstChildElement("Item");
	while(itemNode){
		QString icon, text, userData;
		getElementText(itemNode, "Icon", icon);
		getElementText(itemNode, "Text", text);
		getElementText(itemNode, "UserData", userData);
		comboBox->addItem(QIcon(icon), text, userData);
		itemNode = itemNode->NextSiblingElement("Item");
	}
	bool b;
	int i, width, height;
	QString s;
	if(getElementBool(xmlNode, "Editable", b)) comboBox->setEditable(b);
	if(getElementInt(xmlNode, "CurrentIndex", i)) comboBox->setCurrentIndex(i);
	if(getElementBool(xmlNode, "DuplicatesEnabled", b)) comboBox->setDuplicatesEnabled(b);
	if(getElementBool(xmlNode, "Frame", b)) comboBox->setFrame(b);
	if(getElementInt(xmlNode, "IconWidth", width) && getElementInt(xmlNode, "IconHeight", height)) comboBox->setIconSize(QSize(width, height));
	if(getElementInt(xmlNode, "MaxCount", i)) comboBox->setMaxCount(i);
	if(getElementInt(xmlNode, "MaxVisibleItems", i)) comboBox->setMaxVisibleItems(i);
	if(getElementInt(xmlNode, "MinimumContentsLength", i)) comboBox->setMinimumContentsLength(i);
	connectSignals(xmlNode, comboBox, "currentIndexChanged ( int )");
	return comboBox;
}

QSpinBox *Runner::loadSpinBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	getElementText(xmlNode, "Type", type);
	if(type.toLower() != "spinbox") return 0;
	QSpinBox *spinBox = new QSpinBox(this);
	loadWidget(spinBox, xmlNode);
	int i;
	QString s;
	if(getElementInt(xmlNode, "Minimum", i)) spinBox->setMinimum(i);
	if(getElementInt(xmlNode, "Maximum", i)) spinBox->setMaximum(i);
	if(getElementInt(xmlNode, "SingleStep", i)) spinBox->setSingleStep(i);
	if(getElementInt(xmlNode, "Value", i)) spinBox->setValue(i);
	if(getElementText(xmlNode, "Prefix", s)) spinBox->setPrefix(s);
	if(getElementText(xmlNode, "Suffix", s)) spinBox->setSuffix(s);
	connectSignals(xmlNode, spinBox, "valueChanged ( int )");
	return spinBox;
}

QFontComboBox *Runner::loadFontComboBox(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	if(!getElementText(xmlNode, "Type", type)) return 0;
	if(type.toLower() != "fontcombobox") return 0;
	QFontComboBox *fontCombo = new QFontComboBox(this);
	loadWidget(fontCombo, xmlNode);
	bool b;
	int i, width, height;
	QString s;
	if(getElementBool(xmlNode, "Editable", b)) fontCombo->setEditable(b);
	if(getElementBool(xmlNode, "DuplicatesEnabled", b)) fontCombo->setDuplicatesEnabled(b);
	if(getElementBool(xmlNode, "Frame", b)) fontCombo->setFrame(b);
	if(getElementInt(xmlNode, "IconWidth", width) && getElementInt(xmlNode, "IconHeight", height)) fontCombo->setIconSize(QSize(width, height));
	if(getElementInt(xmlNode, "MaxVisibleItems", i)) fontCombo->setMaxVisibleItems(i);
	if(getElementInt(xmlNode, "MinimumContentsLength", i)) fontCombo->setMinimumContentsLength(i);
	if(getElementText(xmlNode, "FontFilters", s)) fontCombo->setFontFilters(toFontFilters(s));
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
	QString type;
	getElementText(xmlNode, "Type", type);
	if(type.toLower() != "label") return 0;
	QLabel *label = new QLabel(this);
	loadWidget(label, xmlNode);
	bool b;
	QString s;
	if(getElementText(xmlNode, "Align", s)) label->setAlignment(toAlignment(s));
	if(getElementBool(xmlNode, "OpenExternalLinks", b)) label->setOpenExternalLinks(b);
	if(getElementText(xmlNode, "Text", s)) label->setText(s);
	if(getElementText(xmlNode, "Pixmap", s)){
		QPixmap pixmap(s);
		if(!pixmap.isNull()){
			label->setPixmap(pixmap);
		}
	}
	if(getElementBool(xmlNode, "ScaledContents", b)) label->setScaledContents(b);
	if(getElementBool(xmlNode, "WordWrap", b)) label->setWordWrap(b);
	connectSignals(xmlNode, label, "linkActivated ( const QString & )");
	return label;
}

QProgressBar *Runner::loadProgressBar(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	if(!getElementText(xmlNode, "Type", type)) return 0;
	if(type.toLower() != "progressbar") return 0;
	QProgressBar *progressBar = new QProgressBar(this);
	loadWidget(progressBar, xmlNode);
	QString s;
	int i;
	bool b;
	if(getElementText(xmlNode, "Alignment", s)) progressBar->setAlignment(toAlignment(s));
	if(getElementText(xmlNode, "Orientation", s)) progressBar->setOrientation(toOrientation(s));
	if(getElementInt(xmlNode, "Minimum", i)) progressBar->setMinimum(i);
	if(getElementInt(xmlNode, "Maximum", i)) progressBar->setMaximum(i);
	if(getElementInt(xmlNode, "Value", i)) progressBar->setValue(i);
	if(getElementBool(xmlNode, "TextVisible", b)) progressBar->setTextVisible(b);
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
	QString type;
	getElementText(xmlNode, "Type", type);
	if(type.toLower() != "scrollbar") return 0;
	QScrollBar *scrollBar = new QScrollBar(this);
	loadAbstractSlider(scrollBar, xmlNode);
	connectSignals(xmlNode, scrollBar, "sliderReleased ()");
	return scrollBar;
}

void Runner::loadAbstractSlider(QAbstractSlider *abstractSlider, TiXmlElement *xmlNode){
	if(abstractSlider == 0 || xmlNode == 0) return;
	loadWidget(abstractSlider, xmlNode);
	int i;
	QString s;
	bool b;
	if(getElementInt(xmlNode, "Minimum", i)) abstractSlider->setMinimum(i);
	if(getElementInt(xmlNode, "Maximum", i)) abstractSlider->setMaximum(i);
	if(getElementText(xmlNode, "Orientation", s)) abstractSlider->setOrientation(toOrientation(s));
	if(getElementInt(xmlNode, "PageStep", i)) abstractSlider->setPageStep(i);
	if(getElementInt(xmlNode, "SingleStep", i)) abstractSlider->setSingleStep(i);
	if(getElementBool(xmlNode, "Tracking", b)) abstractSlider->setTracking(b);
	if(getElementInt(xmlNode, "Value", i)) abstractSlider->setValue(i);
}

QSlider *Runner::loadSlider(TiXmlElement *xmlNode){
	if(xmlNode == 0) return 0;
	if(!isUse(xmlNode)) return 0;
	QString type;
	getElementText(xmlNode, "Type", type);
	if(type.toLower() != "slider") return 0;
	QSlider *slider = new QSlider(this);
	loadAbstractSlider(slider, xmlNode);
	int i;
	QString s;
	if(getElementText(xmlNode, "TickPosition", s)) slider->setTickPosition(toTickPosition(s));
	if(getElementInt(xmlNode, "TickInterval", i)) slider->setTickInterval(i);
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
	QString type;
	getElementText(xmlNode, "Type", type);
	if(type.toLower() != "ribbonsliderpane") return 0;
	RibbonSliderPane *sliderPane = new RibbonSliderPane(this);
	loadWidget(sliderPane, xmlNode);
	int i, min, max;
	bool b;
	if(getElementInt(xmlNode, "SingleStep", i)) sliderPane->setSingleStep(i);
	if(getElementInt(xmlNode, "Value", i)) sliderPane->setSingleStep(i);
	if(getElementInt(xmlNode, "SliderPosition", i)) sliderPane->setSliderPosition(i);
	if(getElementInt(xmlNode, "Minimum", min) && getElementInt(xmlNode, "Maximum", max)) sliderPane->setRange(min, max);
	if(getElementBool(xmlNode, "ScrollButtons", b)) sliderPane->setScrollButtons(b);
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

void Runner::loadComponents(TiXmlElement *xmlNode){
	if(xmlNode == 0) return;
	if(xmlNode->Value() != QString("Components")) return;
	TiXmlElement *comNode = xmlNode->FirstChildElement("Component");
	while(comNode){
		QString sLib;
		if(!getElementText(comNode, "Library", sLib)){
			LOG_INFO << tr("component not config Library, row number=").toStdString() << comNode->Row();
			comNode = comNode->NextSiblingElement("Component");
			continue;
		}
		QPluginLoader loader("..\\components\\" + sLib + ".dll");
		if(!loader.load()){
			LOG_INFO << tr("load library %1 failed").arg(sLib).toStdString();
			comNode = comNode->NextSiblingElement("Component");
			continue;
		}
		QObject *com = loader.instance();
		if(com == 0){
			LOG_INFO << tr("can not create instance").toStdString();
			comNode = comNode->NextSiblingElement("Component");
			loader.unload();
			continue;
		}
		if(m_components.contains(com)){
			LOG_INFO << tr("config component lib=%1 more than once, row number=%2").arg(sLib).arg(comNode->Row()).toStdString();
			comNode = comNode->NextSiblingElement("Component");
			continue;	
		}
		QString name;
		if(getElementText(comNode, "ObjectName", name)){
			com->setObjectName(name);
		}
		m_components << com;
		com->setParent(this);
		comNode = comNode->NextSiblingElement("Component");
	}
}