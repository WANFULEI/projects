/****************************************************************************
**
** Qtitan Framework by Developer Machines
** 
** Copyright (c) 2009-2012 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QUndoStack>
#include <qdebug.h>
#include <QMenu>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>

#include "QtnRibbonStyleDsgnPlugin.h"

using namespace Qtitan;

static void getAllChildren(QWidget* widget, QWidgetList& list)
{
    list.append(widget);

    QList<QWidget*> widgets = qFindChildren<QWidget*>(widget);
    foreach (QWidget* w, widgets)
        ::getAllChildren(w, list);
}

/* DesignerStyleInput */
DesignerStyleInput::DesignerStyleInput(QWidget* widget) 
    : QInputContext(widget), m_widget(widget) 
{
    m_actRemoveStyle = new QAction(tr("Remove Style"), this);
    connect(m_actRemoveStyle, SIGNAL(triggered()), this, SLOT(removeStyle()));
}

bool DesignerStyleInput::filterEvent(const QEvent* event)
{
    if (event->type()== QEvent::MouseButtonPress)
    {
        QMouseEvent* me = (QMouseEvent *)event;
        if (me->button() == Qt::LeftButton)
        {
            QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(m_widget);
            formWindow->clearSelection(false);
            formWindow->core()->propertyEditor()->setObject(m_widget);
            formWindow->core()->propertyEditor()->setEnabled(true);
            return true;
        }
        else if (me->button() == Qt::RightButton)
        {
            QList<QAction *> actions;
            actions.append(m_actRemoveStyle);
            QMenu::exec(actions, QCursor::pos());
            return true;
        }
    }
    return true; // Handle all input context events here.
}

void DesignerStyleInput::removeStyle()
{
    if (QWidget* widget = qobject_cast<QWidget*>(parent()))
    {
        QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(m_widget);
        QDesignerFormEditorInterface* core = formWindow->core();
        QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());
        container->remove(container->count() - 1);
    }
}

/* DesRibbonStyle */
class DesRibbonStyle : public Qtitan::RibbonStyle
{
public:
    DesRibbonStyle() : Qtitan::RibbonStyle()  { m_parent = Q_NULL; }
public:
    virtual QWidgetList getAllWidgets() const
    {
        QWidgetList list;
        if (m_parent)
            ::getAllChildren(m_parent, list);

        return list;
    }
public:
    QWidget* m_parent;
private:
};


RibbonStyleWidget::RibbonStyleWidget(QWidget* parent) : QWidget(parent) 
{ 
    m_targetStyle = new DesRibbonStyle();
    setAttribute(Qt::WA_InputMethodEnabled);
    DesignerStyleInput* input = new DesignerStyleInput(this);
    setInputContext(input);
}

RibbonStyleWidget::~RibbonStyleWidget()
{
    Q_DELETE_AND_NULL(m_targetStyle);
}

bool RibbonStyleWidget::event(QEvent* event)
{
    bool res = QWidget::event(event);
    if (event->type() == QEvent::ParentChange)
    {
        if (DesRibbonStyle* desStyle = dynamic_cast<DesRibbonStyle*>(m_targetStyle))
            desStyle->m_parent = parentWidget() ;
    }
    return res;
}

void RibbonStyleWidget::paintEvent(QPaintEvent* /*e*/)
{
    QPainter painter(this);
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(width() + width(), 0));
    linearGrad.setColorAt(0, QColor("#FFFFCC"));
    linearGrad.setColorAt(1, Qt::transparent);
    QBrush brush = linearGrad;
    painter.fillRect(QRect(0, 0, width(), height()), brush);
    QRect border = QRect(0, 0, width() - 1, height() - 1);
    painter.drawRect(border);
    painter.rotate(-90);
    painter.translate(-height(), 0);
    QFont f = painter.font();
    f.setBold(true);
    painter.setFont(f);
    QRect area = QRect(1, 1, height() - 1, width() - 1);
    painter.drawText(area, Qt::AlignCenter, "Ribbon Style");
}

QStyle* RibbonStyleWidget::targetStyle() const
{
    return m_targetStyle;
}

QSize RibbonStyleWidget::minimumSizeHint() const
{
    return QSize(20, 100);
}

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Qtitan__RibbonStyleFake[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Qtitan__RibbonStyleFake[] = {
    "Qtitan::RibbonStyle\0"
};

const QMetaObject Qtitan::RibbonStyleFake::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qtitan__RibbonStyleFake,
      qt_meta_data_Qtitan__RibbonStyleFake, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Qtitan::RibbonStyleFake::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Qtitan::RibbonStyleFake::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

/* DesRibbonStyle */
class DesOfficeStyle : public Qtitan::OfficeStyle
{
public:
    DesOfficeStyle() : Qtitan::OfficeStyle() { m_parent = Q_NULL; }
public:
    virtual QWidgetList getAllWidgets() const
    {
        QWidgetList list;
        if (m_parent)
            ::getAllChildren(m_parent, list);

        return list;
    }
public:
    QWidget* m_parent;
private:
};

/* OfficeStyleWidget */
OfficeStyleWidget::OfficeStyleWidget(QWidget* parent)
    : QWidget(parent) 
{ 
    m_targetStyle = new DesOfficeStyle();
    setAttribute(Qt::WA_InputMethodEnabled);
    DesignerStyleInput* input = new DesignerStyleInput(this);
    setInputContext(input);
}

OfficeStyleWidget::~OfficeStyleWidget()
{
    Q_DELETE_AND_NULL(m_targetStyle);
}

QStyle* OfficeStyleWidget::targetStyle() const
{
    return m_targetStyle;
}

QSize OfficeStyleWidget::minimumSizeHint() const
{
    return QSize(20, 150);
}

bool OfficeStyleWidget::event(QEvent* event)
{
    bool res = QWidget::event(event);
    if (event->type() == QEvent::ParentChange)
    {
        if (DesOfficeStyle* desStyle = dynamic_cast<DesOfficeStyle*>(m_targetStyle))
            desStyle->m_parent = parentWidget();
    }
    return res;
}

void OfficeStyleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(width() + width(), 0));
    linearGrad.setColorAt(0, QColor("#FFFFCC"));
    linearGrad.setColorAt(1, Qt::transparent);
    QBrush brush = linearGrad;
    painter.fillRect(QRect(0, 0, width(), height()), brush);
    QRect border = QRect(0, 0, width() - 1, height() - 1);
    painter.drawRect(border);
    painter.rotate(-90);
    painter.translate(-height(), 0);
    QFont f = painter.font();
    f.setBold(true);
    painter.setFont(f);
    QRect area = QRect(1, 1, height() - 1, width() - 1);
    painter.drawText(area, Qt::AlignCenter, "Microsoft Office Style");
}

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Qtitan__OfficeStyleFake[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Qtitan__OfficeStyleFake[] = {
    "Qtitan::OfficeStyle\0"
};

const QMetaObject Qtitan::OfficeStyleFake::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qtitan__OfficeStyleFake,
      qt_meta_data_Qtitan__OfficeStyleFake, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Qtitan::OfficeStyleFake::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Qtitan::OfficeStyleFake::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

/* RibbonStyleFake */
RibbonStyleFake::RibbonStyleFake(QWidget* parent) 
    : RibbonStyleWidget(parent)
{
}

RibbonStyleFake::~RibbonStyleFake()
{
}

/* OfficeStyleFake */
OfficeStyleFake::OfficeStyleFake(QWidget* parent) 
    : OfficeStyleWidget(parent)
{
}

OfficeStyleFake::~OfficeStyleFake()
{
}

QWidget* Qtitan::create_fake_ribbon_style_widget(QWidget* parent)
{
    RibbonStyleFake* retval = new RibbonStyleFake(parent);
    retval->setMaximumSize(QSize(20, 100));
    retval->setMinimumSize(QSize(20, 100));
    return retval;
}

QWidget* Qtitan::create_fake_office_style_widget(QWidget* parent)
{
    OfficeStyleFake* retval = new OfficeStyleFake(parent);
    retval->setMaximumSize(QSize(20, 150));
    retval->setMinimumSize(QSize(20, 150));
    return retval;
}

/* ExtensionManagerWrapper */
ExtensionManagerWrapper::ExtensionManagerWrapper(QExtensionManager *nativeManager)
    : QExtensionManager(nativeManager->parent()), m_nativeManager(nativeManager)
{
    nativeManager->setParent(this);
}

ExtensionManagerWrapper::~ExtensionManagerWrapper()
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    m_nativeManager = Q_NULL;
}

void ExtensionManagerWrapper::setStyleExtentions()
{
    QExtensionManager::registerExtensions(new DesignerMainWindowStyleContainerFactory(this), Q_TYPEID(QDesignerContainerExtension));
}

QExtensionManager* ExtensionManagerWrapper::nativeManager() const
{
    return m_nativeManager;
}

void ExtensionManagerWrapper::registerExtensions(QAbstractExtensionFactory *factory, const QString &iid)
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    if (m_nativeManager != Q_NULL)
        m_nativeManager->registerExtensions(factory, iid);
}

void ExtensionManagerWrapper::unregisterExtensions(QAbstractExtensionFactory *factory, const QString &iid)
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    if (m_nativeManager != Q_NULL)
        m_nativeManager->unregisterExtensions(factory, iid);
}

QObject* ExtensionManagerWrapper::extension(QObject *object, const QString &iid) const
{
    Q_ASSERT(m_nativeManager != Q_NULL);
    if (m_nativeManager != Q_NULL)
    {
        if (object != Q_NULL && object->metaObject()->className() == QString("Qtitan::RibbonStyle"))
            object = static_cast<RibbonStyleWidget *>(object)->targetStyle();
        if (object != Q_NULL && object->metaObject()->className() == QString("Qtitan::OfficeStyle"))
            object = static_cast<OfficeStyleWidget *>(object)->targetStyle();

        QObject* extension = m_nativeManager->extension(object, iid);
        if (iid == Q_TYPEID(QDesignerContainerExtension) && extension != Q_NULL && qobject_cast<QMainWindow *>(object) != 0)
            extension = QExtensionManager::extension(object, iid);

        return extension;
    }
    else
        return Q_NULL;
}


DesignerMainWindowStyleContainer::DesignerMainWindowStyleContainer(
    QDesignerContainerExtension* nativeContainer, QMainWindow* mainWindow, QObject* parent)
        : QObject(parent), m_nativeContainer(nativeContainer), m_mainWindow(mainWindow), m_styleWidget(Q_NULL)
{
}

static void setChildStyle(QWidget* widget, QStyle* style)
{
    widget->setStyle(style);

    QList<QWidget*> widgets = qFindChildren<QWidget*>(widget);
    foreach (QWidget* w, widgets)
        ::setChildStyle(w, style);
}

void DesignerMainWindowStyleContainer::addWidget(QWidget *widget)
{
    if (widget->metaObject()->className() == QString("Qtitan::RibbonStyle"))
    {
        if (m_styleWidget != Q_NULL)
            remove(count() - 1);
        Q_ASSERT(m_styleWidget == Q_NULL);

        RibbonStyleWidget* styleWidget = static_cast<RibbonStyleWidget *>(widget);
        m_styleWidget = styleWidget;
        styleWidget->move(0, 200);

        if (m_mainWindow->metaObject()->className() == QString("Qtitan::RibbonMainWindow"))
        {
            styleWidget->setParent(m_mainWindow->parentWidget());
            styleWidget->setVisible(false);
        }
        else
        {
            styleWidget->setParent(m_mainWindow);
            styleWidget->setVisible(true);
        }

        setChildStyle(m_mainWindow, styleWidget->targetStyle());
        return;
    }
    else if (widget->metaObject()->className() == QString("Qtitan::OfficeStyle"))
    {
        if (m_styleWidget != Q_NULL)
            remove(count() - 1);
        Q_ASSERT(m_styleWidget == Q_NULL);

        OfficeStyleWidget* styleWidget = static_cast<OfficeStyleWidget *>(widget);
        m_styleWidget = styleWidget;
        styleWidget->move(0, 200);
        styleWidget->setParent(m_mainWindow);
        styleWidget->show();
        setChildStyle(m_mainWindow, styleWidget->targetStyle());
        return;
    }

    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->addWidget(widget);

    if (dynamic_cast<RibbonStyleWidget*>(m_styleWidget))
        ::setChildStyle(widget, ((RibbonStyleWidget*)m_styleWidget)->targetStyle());
}

int DesignerMainWindowStyleContainer::count() const
{
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        return m_nativeContainer->count() + (m_styleWidget != Q_NULL ? 1 : 0);
    else
        return -1;
}

int DesignerMainWindowStyleContainer::currentIndex() const
{
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        return m_nativeContainer->currentIndex();
    else
        return -1;
}

void DesignerMainWindowStyleContainer::insertWidget(int index, QWidget *widget)
{
    if (widget->metaObject()->className() == QString("Qtitan::RibbonStyle") ||
        widget->metaObject()->className() == QString("Qtitan::OfficeStyle"))
    {
        addWidget(widget);
        return;
    }
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->insertWidget(index, widget);
}

void DesignerMainWindowStyleContainer::remove(int index)
{
    if (m_styleWidget != Q_NULL && index == count() - 1)
    {
        m_mainWindow->setStyle(Q_NULL);
        m_styleWidget->deleteLater();
        m_styleWidget = Q_NULL;
        return;
    }
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->remove(index);
}

void DesignerMainWindowStyleContainer::setCurrentIndex(int index)
{
    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        m_nativeContainer->setCurrentIndex(index);
}

QWidget* DesignerMainWindowStyleContainer::widget(int index) const
{
    if (m_styleWidget != Q_NULL && index == count() - 1)
        return m_styleWidget;

    Q_ASSERT(m_nativeContainer != Q_NULL);
    if (m_nativeContainer != Q_NULL)
        return m_nativeContainer->widget(index);
    else
        return Q_NULL;
}

QMainWindow* DesignerMainWindowStyleContainer::mainWindow() const
{
    return m_mainWindow;
}


/* DesignerMainWindowStyleContainerFactory */
DesignerMainWindowStyleContainerFactory::DesignerMainWindowStyleContainerFactory(ExtensionManagerWrapper* parent)
    : QExtensionFactory(parent), m_nativeManager(parent->nativeManager())
{
}

QObject* DesignerMainWindowStyleContainerFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension))
        return Q_NULL;
    QObject* extension = m_nativeManager->extension(object, iid);
    return new DesignerMainWindowStyleContainer(
        qobject_cast<QDesignerContainerExtension *>(extension), qobject_cast<QMainWindow *>(object), parent);
}


//////////////////////////////////////////////////////
/* OfficeStyleDsgnPlugin */
OfficeStyleDsgnPlugin::OfficeStyleDsgnPlugin(QObject *parent)
  : QObject(parent), m_core(0)
{
    initialized = false;
}

bool OfficeStyleDsgnPlugin::isContainer() const
{
    return false;
}

bool OfficeStyleDsgnPlugin::isInitialized() const
{
    return initialized;
}

QIcon OfficeStyleDsgnPlugin::icon() const
{
    return QIcon();
}

QString OfficeStyleDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"Qtitan::OfficeStyle\" name=\"officeStyle\">\n"
        " </widget>\n"
        " <customwidgets>\n"
        "     <customwidget>\n"
        "         <class>Qtitan::OfficeStyle</class>\n"
        "         <extends>Q3DockWindow</extends>\n"
        "     </customwidget>\n"
        " </customwidgets>\n"
        "</ui>\n";
}

QString OfficeStyleDsgnPlugin::group() const
{
    return "Developer Machines: Styles";
}

QString OfficeStyleDsgnPlugin::includeFile() const
{
    return "QtnRibbonStyle.h";
}

QString OfficeStyleDsgnPlugin::name() const
{
    return "Qtitan::OfficeStyle";
}

QString OfficeStyleDsgnPlugin::toolTip() const
{
    return "";
}

QString OfficeStyleDsgnPlugin::whatsThis() const
{
    return "";
}

QWidget* OfficeStyleDsgnPlugin::createWidget(QWidget *parent)
{
    return create_fake_office_style_widget(parent);
}

void OfficeStyleDsgnPlugin::formWindowAdded(QDesignerFormWindowInterface *formWindow)
{
    connect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void OfficeStyleDsgnPlugin::formWindowRemoved(QDesignerFormWindowInterface *formWindow)
{
    disconnect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void OfficeStyleDsgnPlugin::widgetManaged(QWidget* widget)
{
    QDesignerFormWindowInterface* formWindow = static_cast<QDesignerFormWindowInterface *>(sender());
    QDesignerFormEditorInterface* core = formWindow->core();
    QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());

    if (widget->metaObject()->className() == QString("Qtitan::OfficeStyle"))
    {
        formWindow->unmanageWidget(widget);

        QUndoStack* stack = formWindow->commandHistory();
        if (!stack->isClean())
        {
            //This code check the InsertWidget command on the stack.
            const QUndoCommand* command = stack->command(stack->index());
            if (command->childCount() == 0)
                return;
        }

        if (qobject_cast<QMainWindow *>(formWindow->mainContainer()) == 0)
        {
            QMessageBox::critical(formWindow->mainContainer(), tr("Can't add Office Style"), 
                tr("You can't drag-drop the style to this QWidget form. The style can be placed only onto the form of QMainWindow successor."));
            widget->deleteLater();
            return;
        }

        for (int i = 0; i < container->count(); ++i)
        {
            QWidget* w = container->widget(i);
            if (w->metaObject()->className() == QString("Qtitan::OfficeStyle"))
            {
                QMessageBox::critical(
                    formWindow->mainContainer(),
                    tr("Can't add Microsoft Office Style"), 
                    tr("Only one instance of the Microsoft Office Style can be adding to the main form."));
                widget->deleteLater();
                return;
            }
        }

        container->addWidget(widget);
        formWindow->core()->metaDataBase()->add(widget);
        widget->setCursor(Qt::PointingHandCursor);
    }
    else if (DesignerMainWindowStyleContainer* styelContainer = dynamic_cast<DesignerMainWindowStyleContainer*>(container))
    {
        if (QMainWindow* mainindow = static_cast<QMainWindow*>(styelContainer->mainWindow()))
        {
            if (Qtitan::CommonStyle* currentStyle = qobject_cast<Qtitan::CommonStyle*>(mainindow->style()))
                ::setChildStyle(widget, currentStyle);
        }
    }
}

void OfficeStyleDsgnPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;
    connect(core->formWindowManager(), SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), 
        this, SLOT(formWindowAdded(QDesignerFormWindowInterface*)));
}

/* RibbonStyleDsgnPlugin */
RibbonStyleDsgnPlugin::RibbonStyleDsgnPlugin(QObject *parent)
  : QObject(parent), m_core(0)
{
    initialized = false;
}

bool RibbonStyleDsgnPlugin::isContainer() const
{
    return false;
}

bool RibbonStyleDsgnPlugin::isInitialized() const
{
    return initialized;
}

QIcon RibbonStyleDsgnPlugin::icon() const
{
    return QIcon();
}

QString RibbonStyleDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"Qtitan::RibbonStyle\" name=\"ribbonStyle\">\n"
        " </widget>\n"
        " <customwidgets>\n"
        "     <customwidget>\n"
        "         <class>Qtitan::RibbonStyle</class>\n"
        "         <extends>Q3DockWindow</extends>\n"
        "     </customwidget>\n"
        " </customwidgets>\n"
        "</ui>\n";
}

QString RibbonStyleDsgnPlugin::group() const
{
    return "Developer Machines: Styles";
}

QString RibbonStyleDsgnPlugin::includeFile() const
{
    return "QtnRibbonStyle.h";
}

QString RibbonStyleDsgnPlugin::name() const
{
    return "Qtitan::RibbonStyle";
}

QString RibbonStyleDsgnPlugin::toolTip() const
{
    return "";
}

QString RibbonStyleDsgnPlugin::whatsThis() const
{
    return "";
}

QWidget* RibbonStyleDsgnPlugin::createWidget(QWidget* parent)
{
    return create_fake_ribbon_style_widget(parent);
}

void RibbonStyleDsgnPlugin::formWindowAdded(QDesignerFormWindowInterface *formWindow)
{
    connect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void RibbonStyleDsgnPlugin::formWindowRemoved(QDesignerFormWindowInterface *formWindow)
{
    disconnect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void RibbonStyleDsgnPlugin::widgetManaged(QWidget* widget)
{
    QDesignerFormWindowInterface* formWindow = static_cast<QDesignerFormWindowInterface *>(sender());
    QDesignerFormEditorInterface* core = formWindow->core();
    QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());

    if (widget->metaObject()->className() == QString("Qtitan::RibbonStyle"))
    {
        formWindow->unmanageWidget(widget);

        QUndoStack* stack = formWindow->commandHistory();
        if (!stack->isClean())
        {
            //This code check the InsertWidget command on the stack.
            const QUndoCommand* command = stack->command(stack->index());
            if (command->childCount() == 0)
                return;
        }

        if (qobject_cast<QMainWindow *>(formWindow->mainContainer()) == 0)
        {
            QMessageBox::critical(formWindow->mainContainer(), tr("Can't add Ribbon Style"), 
                tr("You can't drag-drop the style to this QWidget form. The style can be placed only onto the form of QMainWindow successor."));
            widget->deleteLater();
            return;
        }

        for (int i = 0; i < container->count(); ++i)
        {
            QWidget* w = container->widget(i);
            if (w->metaObject()->className() == QString("Qtitan::RibbonStyle"))
            {
                QMessageBox::critical(
                    formWindow->mainContainer(),
                    tr("Can't add Ribbon Style"), 
                    tr("Only one instance of the Ribbon Style can be adding to the main form."));
                widget->deleteLater();
                return;
            }
        }

        container->addWidget(widget);
        formWindow->core()->metaDataBase()->add(widget);
        widget->setCursor(Qt::PointingHandCursor);
    }
    else if (DesignerMainWindowStyleContainer* styelContainer = dynamic_cast<DesignerMainWindowStyleContainer*>(container))
    {
        if (QMainWindow* mainindow = static_cast<QMainWindow*>(styelContainer->mainWindow()))
        {
            if (Qtitan::CommonStyle* currentStyle = qobject_cast<Qtitan::CommonStyle*>(mainindow->style()))
                ::setChildStyle(widget, currentStyle);
        }
    }
}

class HackDesignerFormEditorInterface: public QDesignerFormEditorInterface
{
    friend class Qtitan::RibbonStyleDsgnPlugin;
public:
    HackDesignerFormEditorInterface() {}
    void hackSetExtensionManager(QExtensionManager* extensionManager)
    {
        setExtensionManager(extensionManager);
    }
};

void RibbonStyleDsgnPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;
    connect(core->formWindowManager(), SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), 
        this, SLOT(formWindowAdded(QDesignerFormWindowInterface*)));

    QExtensionManager* manager = core->extensionManager();
    Q_ASSERT(manager != Q_NULL);
    ExtensionManagerWrapper* wrapper = new ExtensionManagerWrapper(manager);
    ((HackDesignerFormEditorInterface*)core)->hackSetExtensionManager(wrapper);
//    ((HackDesignerFormEditorInterface*)core)->setExtensionManager(wrapper);
    wrapper->setStyleExtentions();
}

