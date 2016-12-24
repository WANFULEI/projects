/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonQuickAccessBar for Qt)
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
#include <QApplication>
#include <QStyleOption>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <qevent.h>

#include "QtnRibbonStyle.h"
#include "QtnRibbonQuickAccessBar.h"

using namespace Qtitan;

namespace Qtitan
{
    /* RibbonQuickAccessButton */
    class RibbonQuickAccessButton : public QToolButton
    {
    public:
        RibbonQuickAccessButton(QWidget* parent = Q_NULL);
        virtual ~RibbonQuickAccessButton();

    public:
        virtual QSize sizeHint() const;

    protected:
        virtual void paintEvent(QPaintEvent*);

    private:
        Q_DISABLE_COPY(RibbonQuickAccessButton)
    };
};

RibbonQuickAccessButton::RibbonQuickAccessButton(QWidget* parent)
    : QToolButton(parent)
{
    setObjectName(QLatin1String("QtnRibbonQuickAccessButton"));
}

RibbonQuickAccessButton::~RibbonQuickAccessButton()
{
}

QSize RibbonQuickAccessButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    if (sz.isNull())
        sz = QSize(16, 16);
//    return QSize((sz.height()*2)/3, sz.height());
    return QSize(sz.height()*13/sz.width()+1, sz.height());
}

void RibbonQuickAccessButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonQuickAccessButton, &opt, &p, this);
}


namespace Qtitan
{
    /* ActionWrapper */
    class ActionWrapper : public QAction
    {
    public:
        ActionWrapper(QObject* p, QAction* srcAction)
            : QAction(srcAction->text(), p)
            , m_srcAction(srcAction)
        {
            setCheckable(true);
            QToolBar* toolBar = qobject_cast<QToolBar*>(parent());
            Q_ASSERT(toolBar);
            setChecked(toolBar->widgetForAction(srcAction));
        }
        void update()
        {
            QToolBar* toolBar = qobject_cast<QToolBar*>(parent());
            Q_ASSERT(toolBar);
            setChecked(toolBar->widgetForAction(m_srcAction));
            setText(m_srcAction->text());
        }

    public:
        QAction* m_srcAction;
    };

    /* ActionInvisible */
    class ActionInvisible : public QAction
    {
    public:
        ActionInvisible(QObject* p, QActionGroup* data)
            : QAction(p)
        {
            m_data = data;
            setVisible(false);
        }
    public:
        QActionGroup* m_data;
    };
};


namespace Qtitan
{
    /* RibbonBarPrivate */
    class RibbonQuickAccessBarPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonQuickAccessBar)
    public:
        explicit RibbonQuickAccessBarPrivate();
    
    public:
        void init();
        ActionWrapper* findWrapper(QAction* action) const;
        QAction* findBeforeAction(QAction* action) const;
        void updateAction(QAction* action);
        void setActionVisible(QAction* action, bool visible);

    public:
        RibbonQuickAccessButton* accessPopup_;
        QMenu* menu_;
        QAction* actionAccessPopup_;
        QActionGroup* customizeGroupAction_;
        
        bool removeAction_    : 1;
        bool customizeAction_ : 1;
    };
};

RibbonQuickAccessBarPrivate::RibbonQuickAccessBarPrivate()
{
    accessPopup_ = Q_NULL;
    menu_ = Q_NULL;
    actionAccessPopup_  = Q_NULL;
    customizeGroupAction_ = Q_NULL;
    removeAction_ = false;
    customizeAction_ = false;
}

void RibbonQuickAccessBarPrivate::init()
{
    QTN_P(RibbonQuickAccessBar);

    accessPopup_ = new RibbonQuickAccessButton(&p);
    accessPopup_->setPopupMode(QToolButton::InstantPopup);
    menu_ = new QMenu(&p);
    accessPopup_->setMenu(menu_);

    actionAccessPopup_ = p.addWidget(accessPopup_);

    customizeGroupAction_ = new QActionGroup(&p);
    customizeGroupAction_->setExclusive(false);
    QObject::connect(customizeGroupAction_, SIGNAL(triggered(QAction*)), &p, SLOT(customizeAction(QAction*)));

    p.addAction(new ActionInvisible(&p, customizeGroupAction_));
}

ActionWrapper* RibbonQuickAccessBarPrivate::findWrapper(QAction* action) const
{
    QList<QAction*> list = customizeGroupAction_->actions();
    for (int i = 0; i < list.count(); ++i)
    {
        ActionWrapper* act = dynamic_cast<ActionWrapper*>(list[i]);
        if (act && action == act->m_srcAction)
            return act;
    }
    return Q_NULL;
}

QAction* RibbonQuickAccessBarPrivate::findBeforeAction(QAction* action) const
{
    QList<QAction*> list = customizeGroupAction_->actions();
    bool find = false;
    for (int i = 0, count = list.count(); i < count; ++i)
    {
        if (find)
        {
            if (ActionWrapper* beforeAct = dynamic_cast<ActionWrapper*>(list[i]))
            {
                if (beforeAct->isChecked())
                    return beforeAct->m_srcAction;
            }
        }
        if (!find && action == list[i])
            find = true;
    }
    return Q_NULL;
}

void RibbonQuickAccessBarPrivate::updateAction(QAction* action)
{
    if (ActionWrapper* wrapper = findWrapper(action))
        wrapper->update();
}

void RibbonQuickAccessBarPrivate::setActionVisible(QAction* action, bool visible)
{
    QTN_P(RibbonQuickAccessBar);
    if (ActionWrapper* wrapper = findWrapper(action))
    {
        if (visible)
        {
            if (customizeAction_)
            {
                QAction* beforeAct = findBeforeAction(wrapper);
                p.insertAction(beforeAct, action);
            }
            else
                p.addAction(action);
        }
        else
        {
            p.removeAction(action);
            removeAction_ = false;
        }
        wrapper->update();
        p.adjustSize();
    }
}


/* RibbonQuickAccessBar */
RibbonQuickAccessBar::RibbonQuickAccessBar(QWidget* parent)
    : QToolBar(parent)
{
    QTN_INIT_PRIVATE(RibbonQuickAccessBar);
    QTN_D(RibbonQuickAccessBar);
    d.init();
}

RibbonQuickAccessBar::~RibbonQuickAccessBar()
{
    QTN_FINI_PRIVATE();
}

QAction* RibbonQuickAccessBar::actionCustomizeButton() const
{
    QTN_D(const RibbonQuickAccessBar);
    return d.actionAccessPopup_;
}

void RibbonQuickAccessBar::setActionVisible(QAction* action, bool visible)
{
    QTN_D(RibbonQuickAccessBar);
    d.setActionVisible(action, visible);
}

bool RibbonQuickAccessBar::isActionVisible(QAction* action) const
{
    QTN_D(const RibbonQuickAccessBar);
    if (ActionWrapper* wrapper = d.findWrapper(action))
        return wrapper->isChecked();
    return false;
}

int RibbonQuickAccessBar::getVisibleCount() const
{
    QTN_D(const RibbonQuickAccessBar);
    int visibleCount = 0;
    QList<QAction*> list = d.customizeGroupAction_->actions();
    for (int i = 0, count = list.count(); i < count; ++i)
    {
        if (ActionWrapper* beforeAct = dynamic_cast<ActionWrapper*>(list[i]))
            if (beforeAct->isChecked())
                visibleCount++; 
    }
    return visibleCount;
}

QSize RibbonQuickAccessBar::sizeHint() const
{
    const int heightTabs = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonTabsHeight, 0, 0);
    return QSize(QToolBar::sizeHint().width(), heightTabs+1).expandedTo(QApplication::globalStrut());
}

void RibbonQuickAccessBar::customizeAction(QAction* action)
{
    QTN_D(RibbonQuickAccessBar);
    d.customizeAction_ = true;
    if (ActionWrapper* act = dynamic_cast<ActionWrapper*>(action))
        setActionVisible(act->m_srcAction, !widgetForAction(act->m_srcAction));
    d.customizeAction_ = false;

    QEvent event(QEvent::LayoutRequest);
    QApplication::sendEvent(parentWidget(), &event);
    parentWidget()->update();
}

bool RibbonQuickAccessBar::event(QEvent* event)
{
    if ((QEvent::Hide == event->type() || QEvent::Show == event->type()) && parentWidget())
    {
        adjustSize();
        parentWidget()->adjustSize();
    }
    return QToolBar::event(event);
}

void RibbonQuickAccessBar::actionEvent(QActionEvent* event)
{
    QToolBar::actionEvent(event);

    QTN_D(RibbonQuickAccessBar);
    if (d.actionAccessPopup_)
    {
        if (event->type() == QEvent::ActionAdded)
        {
            if (!d.removeAction_)
                removeAction(d.actionAccessPopup_);

            if (event->action() != d.actionAccessPopup_ && !d.findWrapper(event->action()) && !dynamic_cast<ActionInvisible*>(event->action()))
            {
                ActionWrapper* act = new ActionWrapper(this, event->action());
                d.menu_->addAction(act);
                d.customizeGroupAction_->addAction(act);
                adjustSize();
            }
        }
        else if (event->type() == QEvent::ActionRemoved)
        {
            if (event->action() == d.actionAccessPopup_)
            {
                d.removeAction_ = true;
                addAction(d.actionAccessPopup_);
                d.removeAction_ = false;
            }
            else if (ActionInvisible* actInvisible = dynamic_cast<ActionInvisible*>(event->action()))
            {
                d.menu_->clear();
                QList<QAction*> actList = actInvisible->m_data->actions();
                for (int i = actList.size()-1; i >= 0; i--) 
                {
                    QAction* actionWrapper = actList[i];
                    d.customizeGroupAction_->removeAction(actionWrapper);
                    delete actionWrapper;
                }
                delete actInvisible;
                addAction(new ActionInvisible(this, d.customizeGroupAction_));
            }
            else
                d.updateAction(event->action());

        }
        else if (event->type() == QEvent::ActionChanged)
        {
            if (event->action() == d.actionAccessPopup_)
                d.accessPopup_->setDefaultAction(d.actionAccessPopup_);
            else if(!dynamic_cast<ActionWrapper*>(event->action()))
                d.updateAction(event->action());
        }
    }
}

void RibbonQuickAccessBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
}


