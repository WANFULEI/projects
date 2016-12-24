/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonPage for Qt)
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
#include <QPainter>
#include <QBasicTimer>
#include <QStyleOption>
#include <qevent.h>

#include "QtnRibbonStyle.h"
#include "QtnRibbonPage.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonTabBar.h"
#include "QtnRibbonPrivate.h"

using namespace Qtitan;



namespace Qtitan
{
    /* RibbonPagePrivate */
    class RibbonPagePrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonPage)

        struct InfoGroup
        {
            QSize maxSize;
            QSize minSize;
            RibbonGroupWrapper* group;
            bool bReduced;
            bool visible;
            InfoGroup()
            {
                bReduced = false;
                visible = true;
                group = Q_NULL;
            }
        };
    public:
        explicit RibbonPagePrivate();

    public:
        int calcReducedSumWidth(QList<InfoGroup>& infoGroups) const;
        void calcReducedGroups(int widthPage, QList<InfoGroup>& infoGroups);
        int calcSmartLayoutGroups(int* widths);
        void layoutGroups();
        bool validIndex(int index) const { return index >= 0 && index < listGroups_.count(); }

        void removeGroup(int index);
        int getIndexGroup(RibbonGroup* page) const;

        virtual bool eventFilter(QObject*, QEvent* event);

    public:
        QList<RibbonGroupWrapper*> listGroups_;
        QList<QAction*> listShortcuts_;
        RibbonTab* associativeTab_;
        QBasicTimer timer_;
        QString title_;
        QString contextTitle_;
        QSize sizeGroups_;
        int groupsHeight_;
        bool minimazeRibbon_ : 1;
        bool doVisiblePage_  : 1;
        bool allowPress_     : 1; 
        bool doPopupPage_    : 1;
    };
}

RibbonPagePrivate::RibbonPagePrivate()
{
    minimazeRibbon_ = false;
    doVisiblePage_ = false;  
    allowPress_ = false;
    doPopupPage_ = false;
    associativeTab_ = Q_NULL;
    groupsHeight_ = -1;
}

int RibbonPagePrivate::calcReducedSumWidth(QList<InfoGroup>& infoGroups) const
{
    int sumWidth = 0;
    for (int i = 0, count = infoGroups.size(); count > i; i++) 
    {
        InfoGroup& itemGroup = infoGroups[i];
        if (itemGroup.visible)
        {
            sumWidth += itemGroup.bReduced ? itemGroup.minSize.width() : itemGroup.maxSize.width();
            sumWidth += 4;
        }
    }
    return sumWidth;
}

void RibbonPagePrivate::calcReducedGroups(int widthPage, QList<InfoGroup>& infoGroups)
{
    int sumWidth = calcReducedSumWidth(infoGroups);
    if (sumWidth >= widthPage)
    {
        for (int i = infoGroups.size()-1; i >= 0; i--) 
        {
            InfoGroup& itemGroup = infoGroups[i];
            if (!itemGroup.bReduced)
            {
                itemGroup.bReduced = true;
                sumWidth = calcReducedSumWidth(infoGroups);
                if (!(sumWidth < widthPage))
                    calcReducedGroups(widthPage, infoGroups);
                break;
            }
        }
    }
    else if (sumWidth <= widthPage)
    {
        for (int i = 0, count = infoGroups.size(); count > i; i++) 
        {
            InfoGroup& itemGroup = infoGroups[i];
            if (itemGroup.bReduced)
            {
                itemGroup.bReduced = false;
                sumWidth = calcReducedSumWidth(infoGroups);
                if (sumWidth > widthPage)
                    itemGroup.bReduced = true;
                else
                    calcReducedGroups(widthPage, infoGroups);
                break;
            }
        }
    }
}

int RibbonPagePrivate::calcSmartLayoutGroups(int* widths)
{
    int width = 0;
    for (int i = 0; i < listGroups_.size(); i++)
    {
        width += widths[i];
        if (i != listGroups_.size() - 1 && widths[i] != 0)
            width += 7;
    }
    return width;
}

void RibbonPagePrivate::layoutGroups()
{
    QTN_P(RibbonPage);

    p.setUpdatesEnabled(false);

    int* pData = new int[listGroups_.size()];
    for (int i = 0, count = listGroups_.size(); count > i; i++)
    {
        RibbonGroupWrapper* group = listGroups_.at(i);
        group->updateLayout(Q_NULL, true);
        pData[i] = group->isVisible() ? group->realSize().width() : 0;
    }

    int resultWidth = calcSmartLayoutGroups(pData);

    for (int i = 0, count = listGroups_.size(); count > i; i++)
    {
        RibbonGroupWrapper* group = listGroups_.at(i);
        int width = p.size().width() - resultWidth;
        group->updateLayout(&width, false);
    }

    QList<InfoGroup> infoGroups;
    for (int i = 0, count = listGroups_.size(); count > i; i++)
    {
        RibbonGroupWrapper* group = listGroups_.at(i);
        InfoGroup itemInfo;
        itemInfo.bReduced = group->isReducedGroup();
        itemInfo.visible = group->isVisible();
        itemInfo.maxSize = itemInfo.visible ? group->realSize() : QSize(0,0);
        itemInfo.minSize = itemInfo.visible ? group->reducedSize() : QSize(0,0);
        itemInfo.group = group;
        infoGroups << itemInfo;
    }
    QRect rcPage = p.geometry();
    calcReducedGroups(rcPage.width(), infoGroups);
    
    int x = 4;
    QSize size;
    for (int i = 0, count = infoGroups.size(); count > i; i++)
    {
        InfoGroup& itemGroup = infoGroups[i];
        RibbonGroupWrapper* group = itemGroup.group;

        if (itemGroup.bReduced)
        {
            if (!group->isReducedGroup())
                group->setReducedGroup(true);
        }
        else if (!itemGroup.bReduced)
        {
            if (group->isReducedGroup())
                group->resetReducedGroup();
        }

        QSize sz = itemGroup.visible ? group->sizeHint() : QSize(0, 0);
        if (groupsHeight_ != -1)
            sz.setHeight(groupsHeight_);

        int width = sz.width();

        if (itemGroup.visible )
        {
            group->setGeometry(x, 3, width, sz.height());
            group->getGroup()->setFixedHeight(sz.height());
            x += width + 2;
        }
        QSize sizegroup = itemGroup.visible ? group->sizeHint() : QSize(0, 0);
        size = QSize(sizegroup.width() + size.width(), /*groupsHeight_ != -1 ? groupsHeight_ :*/ sizegroup.height());
    }
    sizeGroups_ = size;

    delete[] pData;

    p.setUpdatesEnabled(true);
}

void RibbonPagePrivate::removeGroup(int index)
{
    if (validIndex(index)) 
    {
        RibbonGroupWrapper* groupWrapper = listGroups_[index];

        groupWrapper->getGroup()->removeEventFilter(this);
        QList<QAction*> actList = groupWrapper->getGroup()->actions();

        for (int i = actList.size()-1; i >= 0; --i) 
        {
            QAction* a = actList.at(i);
            listShortcuts_.removeOne(a);
        }

        listGroups_.removeAt(index);
        delete groupWrapper;
        layoutGroups();
    }
}

int RibbonPagePrivate::getIndexGroup(RibbonGroup* page) const
{
    for (int i = 0, count = listGroups_.size(); count > i; i++) 
    {
        RibbonGroupWrapper* groupWrapper = listGroups_.at(i);
        if (page == groupWrapper->getGroup())
            return i;
    }
    return -1;
}

bool RibbonPagePrivate::eventFilter(QObject* obj, QEvent* event)
{
    bool res = QObject::eventFilter(obj, event);

    if (!qobject_cast<RibbonGroup*>(obj))
        return res;

    switch (event->type()) 
    {
        case QEvent::ActionAdded: 
            if (QActionEvent* actEvent = static_cast<QActionEvent*>(event))
            {
                listShortcuts_.append(actEvent->action());
                if (associativeTab_)
                    associativeTab_->addAction(actEvent->action());
            }
            break;
        case QEvent::ActionRemoved: 
            if (QActionEvent* actEvent = static_cast<QActionEvent*>(event))
            {
                if (associativeTab_)
                    associativeTab_->removeAction(actEvent->action());

                int index = listShortcuts_.indexOf(actEvent->action());
                if (index != -1)
                    listShortcuts_.removeAt(index);
            }
            break;
        default:
            break;
    }

    return res;
}


/* RibbonPage */
RibbonPage::RibbonPage(QWidget* parent)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonPage);
}

RibbonPage::RibbonPage(QWidget* parent, const QString& title)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonPage);
    setTitle(title);
}

RibbonPage::~RibbonPage()
{
    if (RibbonBar* ribbon = qobject_cast<RibbonBar*>(parentWidget()))
        ribbon->detachPage(this);
    QTN_FINI_PRIVATE();
}

bool RibbonPage::isVisible() const
{
    QTN_D(const RibbonPage);
    return QWidget::isVisible() || (d.associativeTab_ && d.associativeTab_->isVisible());
}

RibbonGroup* RibbonPage::addGroup(const QString& groupTitle)
{
    return insertGroup(-1, groupTitle);
}

RibbonGroup* RibbonPage::insertGroup(int index, const QString& groupTitle)
{
    QTN_D(RibbonPage);
    
    setUpdatesEnabled(false);

    RibbonGroupWrapper* wrapper = new RibbonGroupWrapper(this);
    RibbonGroup* group = new RibbonGroup(wrapper, groupTitle);
    wrapper->setGroup(group);
    
    if (!d.validIndex(index)) 
    {
        index = d.listGroups_.count();
        d.listGroups_.append(wrapper);
    } 
    else 
        d.listGroups_.insert(index, wrapper);

    connect(group, SIGNAL(actionTriggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
    connect(group, SIGNAL(released()), this, SLOT(released()));

    wrapper->show();

    group->installEventFilter(&d);

    return group;
}

void RibbonPage::removeGroup(RibbonGroup* group)
{
    QTN_D(RibbonPage);
    removeGroup(d.getIndexGroup(group));
}

void RibbonPage::removeGroup(int index)
{
    QTN_D(RibbonPage);
    d.removeGroup(index);
}

int RibbonPage::groupCount() const
{
    QTN_D(const RibbonPage);
    return d.listGroups_.count();
}

RibbonGroup* RibbonPage::getGroup(int index) const
{
    QTN_D(const RibbonPage);
    if (index < 0 || index >= d.listGroups_.size())
        return Q_NULL;
    return d.listGroups_[index]->getGroup();
}

void RibbonPage::clearGroups()
{
    QTN_D(RibbonPage);
    for(int i = (int)d.listGroups_.count()-1; i >= 0; i--)
        removeGroup(i);
}

void RibbonPage::setContextPage(ContextColor color)
{
    QTN_D(RibbonPage);
    d.associativeTab_->setContextTab(color);
    repaint();
    parentWidget()->repaint();
}

ContextColor RibbonPage::getContextColor() const
{
    QTN_D(const RibbonPage);
    return d.associativeTab_->getContextColor();
}

void RibbonPage::setVisible(bool visible)
{
    QTN_D(RibbonPage);

    bool saveVisible = isVisible();

    if (d.minimazeRibbon_ && d.associativeTab_)
    {
        if (d.doPopupPage_)
            QWidget::setVisible(visible);

        if (!d.doPopupPage_)
        {
            d.associativeTab_->setVisible(visible);
            if (saveVisible != isVisible())
            {
                if (RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(parentWidget()))
                {
                    QEvent ev(QEvent::LayoutRequest);
                    QApplication::sendEvent(ribbonBar, &ev);
                    ribbonBar->update();
                }
            }
        }

        if (!visible && d.doPopupPage_)
            d.doPopupPage_ = false;
    }
    else
    {
        if (d.doVisiblePage_)
            QWidget::setVisible(visible);
        else if (!visible)
            QWidget::setVisible(false);


        if (!d.doVisiblePage_ && d.associativeTab_)
            d.associativeTab_->setVisible(visible);

        if (!d.doVisiblePage_ && !visible)
        {
            if (RibbonBar* ribbon = qobject_cast<RibbonBar*>(parentWidget()))
            {
                if (ribbon->getPageCount() > 0)
                {
                    RibbonPage* page = ribbon->getPage(ribbon->currentIndexPage());
                    if (this == page)
                        ribbon->setCurrentPage(0);
                }
            }
        }

        if (saveVisible != isVisible())
        {
            if (RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(parentWidget()))
            {
                QEvent ev(QEvent::LayoutRequest);
                QApplication::sendEvent(ribbonBar, &ev);
                ribbonBar->update();
            }
        }
    }
}

void RibbonPage::setTitle(const QString& title)
{
    QTN_D(RibbonPage);

    d.title_ = title;

    if (d.associativeTab_)
        d.associativeTab_->setTextTab(d.title_);

    emit titleChanged(d.title_);
}

const QString& RibbonPage::title() const
{
    QTN_D(const RibbonPage);
    return d.associativeTab_ ? d.associativeTab_->textTab() : d.title_;
}

void RibbonPage::setContextTitle(const QString& title)
{
    QTN_D(RibbonPage);
    d.contextTitle_ = title;

    if (d.associativeTab_)
        d.associativeTab_->setContextTextTab(d.contextTitle_);
}

void RibbonPage::released()
{
    QTN_D(RibbonPage);
    if (d.minimazeRibbon_ && isVisible() && QApplication::activePopupWidget() == this)
        hide();
}

void RibbonPage::actionTriggered(QAction* action)
{
    QTN_D(RibbonPage);

    if (d.listShortcuts_.indexOf(action) != -1)
        return;

    if (d.minimazeRibbon_ && isVisible())
    {
        if (action->menu())
            return;
        hide();
    }
}

const QString& RibbonPage::contextTitle() const
{
    QTN_D(const RibbonPage);
    return d.associativeTab_ ? d.associativeTab_->contextTextTab() : d.title_;
}

void RibbonPage::setAssociativeTab(QWidget* widget)
{
    QTN_D(RibbonPage);
    d.associativeTab_ = qobject_cast<RibbonTab*>(widget);

    if (!d.associativeTab_)
        return;

    for (int j = 0, count = groupCount(); count > j; j++)
    {
        if (RibbonGroup* group = getGroup(j))
        {
            QList<QAction*> actList = group->actions();

            for (int i = 0, actCount = actList.size(); actCount > i; i++) 
            {
                QAction* a = actList.at(i);
                d.listShortcuts_.append(a);
                d.associativeTab_->addAction(a);
            }
        }
    }
}

void RibbonPage::setPageVisible(bool visible)
{
    QTN_D(RibbonPage);
    d.doVisiblePage_ = true;
    if (visible && d.associativeTab_ && !d.associativeTab_->isHidden())
    {
        setVisible(true);
        emit activated();
    }
    else if (d.associativeTab_ && !d.associativeTab_->isHidden())
        setVisible(false);
    d.doVisiblePage_ = false;
}

void RibbonPage::popup()
{
    QTN_D(RibbonPage);
    d.doPopupPage_ = true;
//    ensurePolished();
//    d.layoutGroups();
    show();
}

void RibbonPage::setRibbonMinimized(bool minimized)
{
    QTN_D(RibbonPage);
    d.minimazeRibbon_ = minimized;
}

void RibbonPage::activatingPage(bool& allow)
{
    emit activating(allow);
}

void RibbonPage::setGroupsHeight(int height)
{
    QTN_D(RibbonPage);
    d.groupsHeight_ = height;
}

bool RibbonPage::event(QEvent* event)
{
    QTN_D(RibbonPage);
    switch(event->type())
    {
        case QEvent::LayoutRequest :
                d.layoutGroups();
            break;
        case QEvent::Show :
            if (d.minimazeRibbon_ && d.associativeTab_)
            {
                d.associativeTab_->setTrackingMode(true);
                d.timer_.start(QApplication::doubleClickInterval(), this);
                d.allowPress_ = true;
            }
            break;
        case QEvent::Hide :
            if (d.minimazeRibbon_ && d.associativeTab_)
            {
                d.associativeTab_->setTrackingMode(false);
                d.allowPress_ = true;
            }
            break;
        case QEvent::Timer :
            if (d.minimazeRibbon_ && d.associativeTab_)
            {
                QTimerEvent* pTimerEvent = (QTimerEvent*)event;
                if (d.timer_.timerId() == pTimerEvent->timerId()) 
                {
                    d.allowPress_ = false;
                    return true;
                }
            }
            break;
        case QEvent::MouseButtonPress :
            if (d.minimazeRibbon_ && d.associativeTab_)
            {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                if (!rect().contains(mouseEvent->pos())) 
                {
                    if (d.allowPress_)
                    {
                        QWidget* clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());

                        if (clickedWidget == d.associativeTab_)
                        {
                            const QPoint targetPoint = clickedWidget->mapFromGlobal(mouseEvent->globalPos());

                            QMouseEvent evPress(mouseEvent->type(), targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                            QApplication::sendEvent(clickedWidget, &evPress);

                            QMouseEvent eDblClick(QEvent::MouseButtonDblClick, targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                            QApplication::sendEvent(d.associativeTab_, &eDblClick);
                            return true;
                        }
                    }
                    else if (d.associativeTab_ && QRect(d.associativeTab_->mapToGlobal(QPoint()), d.associativeTab_->size()).contains(mouseEvent->globalPos()))
                        setAttribute(Qt::WA_NoMouseReplay);

                    if (QApplication::activePopupWidget() == this)
                    {
                        if (QWidget* clickedTab = qobject_cast<RibbonTab*>(QApplication::widgetAt(mouseEvent->globalPos())))
                        {
                            if (d.associativeTab_ && clickedTab != d.associativeTab_)
                            {
                                const QPoint targetPoint = clickedTab->mapFromGlobal(mouseEvent->globalPos());

                                QMouseEvent evPress(mouseEvent->type(), targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                                QApplication::sendEvent(clickedTab, &evPress);

                                return false;
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
    }

    return QWidget::event(event);
}

QSize RibbonPage::sizeHint() const
{
    QTN_D(const RibbonPage);
    return d.sizeGroups_;
}

void RibbonPage::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonPage);

    if (d.minimazeRibbon_ && this->parentWidget())
    {
        QPainter p(this);
        QStyleOption opt;
        opt.init(this);
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this->parentWidget());
    }
}

void RibbonPage::changeEvent(QEvent* event)
{
    switch (event->type()) 
    {
    case QEvent::FontChange: 
        if (QWidget* parent = parentWidget())
        {
            QEvent ev(QEvent::LayoutRequest);
            QApplication::sendEvent(this, &ev);
            parent->updateGeometry();
        }
        break;
    default:
        break;
    };
    return QWidget::changeEvent(event);;
}

void RibbonPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    QTN_D(RibbonPage);
    d.layoutGroups();
}

void RibbonPage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    QTN_D(RibbonPage);
    if (event->size() != event->oldSize())
        d.layoutGroups();
}




