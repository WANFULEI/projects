/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced OfficePopupMenu for Qt)
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
#include <QWidgetAction>
#include <QPainter>
#include <qevent.h>
#include <QLayout>
#include <qdebug.h>

#include "QtnRibbonStyle.h"
#include "QtnOfficeDefines.h"
#include "QtnOfficePopupMenu.h"

using namespace Qtitan;


namespace Qtitan
{
    /* OfficePopupMenuPrivate */
    class OfficePopupMenuPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(OfficePopupMenu)
    public:
        explicit OfficePopupMenuPrivate();

    public:
        void init();
        int calcMinimumHeight();
        QWidget* findWidget(const char* nameWidget) const;

    public:
        QWidget* widgetPopup_; 
        bool resizable_      : 1;
        bool pressSizeGrip_  : 1;
        bool showGripper_    : 1;
        int lastWidth_;
    };
};

OfficePopupMenuPrivate::OfficePopupMenuPrivate()
{
    widgetPopup_ = Q_NULL;
    resizable_ = false;
    pressSizeGrip_ = false;
    showGripper_ = true;
    lastWidth_ = -1;
}

void OfficePopupMenuPrivate::init()
{
    QTN_P(OfficePopupMenu);
    p.setProperty(qtn_PopupBar, true);
    p.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    p.setMouseTracking(true);
}

int OfficePopupMenuPrivate::calcMinimumHeight()
{
    QTN_P(OfficePopupMenu);

    int height = 11;

    QList<QAction*> listActions = p.actions();
    for (int i = 0; i < listActions.count(); i++) 
    {
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(listActions.at(i)))
        {
            if (QWidget* w = widgetAction->defaultWidget())
            {
                int minHeight = w->minimumSizeHint().height();
                if (minHeight == -1)
                    minHeight = w->sizeHint().height();
                height += minHeight;
            }
        }
        else
            height += p.actionGeometry(listActions.at(i)).height();
    }
    return height;
}

QWidget* OfficePopupMenuPrivate::findWidget(const char* nameWidget) const
{
    QTN_P(const OfficePopupMenu);
    const QObjectList& listChildren = p.children();
    for (int i = 0; i < listChildren.size(); ++i) 
    {
        QWidget* w = qobject_cast<QWidget*>(listChildren.at(i));

        if (w && w->property(nameWidget).toBool() )
            return w;
    }
    return Q_NULL;
}


/* OfficePopupMenu */
OfficePopupMenu::OfficePopupMenu(QWidget* parent)
    : QMenu(parent)
{
    QTN_INIT_PRIVATE(OfficePopupMenu);
    QTN_D(OfficePopupMenu);
    d.init();

    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShowBar()));
}

OfficePopupMenu::~OfficePopupMenu()
{
    QTN_FINI_PRIVATE();
}

OfficePopupMenu* OfficePopupMenu::createPopupMenu(QWidget* parent)
{
    OfficePopupMenu* popupBar = new OfficePopupMenu(parent);
    return popupBar;
}

QAction* OfficePopupMenu::addWidget(QWidget* widget)
{
    QTN_D(OfficePopupMenu);
    QWidgetAction* action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    addAction(action);

    if (widget && widget->property("isResizable").toBool())
        d.resizable_ = true;

    return action;
}

void OfficePopupMenu::setShowGripper(bool show)
{
    QTN_D(OfficePopupMenu);
    d.showGripper_ = show;
}

bool OfficePopupMenu::showGripper() const
{
    QTN_D(const OfficePopupMenu);
    return d.showGripper_ ;
}

void OfficePopupMenu::setWidgetBar(QWidget* widget)
{
    QTN_D(OfficePopupMenu);
    Q_ASSERT(widget != Q_NULL && d.widgetPopup_ == Q_NULL);

    d.widgetPopup_ = widget;
}

void OfficePopupMenu::aboutToShowBar()
{
    QApplication::sendEvent(this, new QResizeEvent(size(), QSize(-1, -1)));
}


QSize OfficePopupMenu::sizeHint() const
{
    QSize size = QMenu::sizeHint();
    
    QTN_D(const OfficePopupMenu);

    int height = 0;
    if (d.resizable_)
        height = 11;// : 8;

    QSize sz;
    if (d.widgetPopup_)
        sz = d.widgetPopup_->geometry().size();
    else if (QWidget* w = d.findWidget(qtn_WidgetGallery))
        sz = w->baseSize();

    size.setWidth(d.lastWidth_ != -1 ? d.lastWidth_ : sz.width());
    size.setHeight(size.height() + height);

    return size;
}

bool OfficePopupMenu::event(QEvent* event)
{
    QTN_D(OfficePopupMenu);

    switch(event->type())
    {
        case QEvent::Show :
                if (d.widgetPopup_ && d.resizable_)
                {
                    setMinimumWidth(d.widgetPopup_->geometry().width());
                    setMinimumHeight(d.calcMinimumHeight());
                }
            break;
        case QEvent::Resize :
            {
                QResizeEvent* resizeEvent = (QResizeEvent*)event;
                const QSize& sz = resizeEvent->size();
                if (!resizeEvent->oldSize().isEmpty() && resizeEvent->oldSize() != sz)
                {
                    if (QWidget* w = d.findWidget(qtn_WidgetGallery))
                    {
                        if (w && !w->isWindow() && !w->isHidden())
                        {
                            QSize sizeWidget = w->baseSize();
                            sizeWidget.setHeight(sizeWidget.height()-(resizeEvent->oldSize().height()-sz.height()));
                            if (sizeWidget != w->geometry().size())
                                w->setBaseSize(sizeWidget);
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
    return QMenu::event(event);
}

void OfficePopupMenu::paintEvent(QPaintEvent* event)
{
    QMenu::paintEvent(event);
    QTN_D(OfficePopupMenu);

    if (d.resizable_)
    {
        QPainter p(this);
        QStyleOptionSizeGrip opt;
        opt.init(this);
        opt.rect.adjust(1, 0, -1, -1);

        opt.rect.setTop(opt.rect.bottom()-11);

        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_PopupSizeGrip, &opt, &p, this);
    }
}

void OfficePopupMenu::mousePressEvent(QMouseEvent* event)
{
    QTN_D(OfficePopupMenu);
    if (d.resizable_)
    {
        if (event->buttons() == Qt::LeftButton) 
        {
            QRect rcResizeGripper(rect());
            rcResizeGripper.setTop(rcResizeGripper.bottom()-11);
            QRect rcResizeGripperAll = rcResizeGripper;
            rcResizeGripper.setLeft(rcResizeGripper.right() - rcResizeGripper.height());
            if (rcResizeGripper.contains(event->pos()))
            {
                if (QWidget* w = d.findWidget(qtn_WidgetGallery))
                {
                    if (w && !w->isWindow() && !w->isHidden())
                        w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                }
                d.pressSizeGrip_ = true;
                return;
            }
            else if (rcResizeGripperAll.contains(event->pos()))
                return;
        }
    }
    QMenu::mousePressEvent(event);
}

void OfficePopupMenu::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(OfficePopupMenu);
    if (d.resizable_)
    {
        QRect rcResizeGripper(rect());
        rcResizeGripper.setTop(rcResizeGripper.bottom()-11);
        rcResizeGripper.setLeft(rcResizeGripper.right() - rcResizeGripper.height());

        if (d.pressSizeGrip_ || rcResizeGripper.contains(event->pos()))
            setCursor(Qt::SizeFDiagCursor);
        else
            unsetCursor();

        if (d.pressSizeGrip_)
        {
            QPoint np(event->globalPos());

            QRect rect = geometry();
            QRect rectOld = rect;

            rect.setRight(np.x());
            rect.setBottom(np.y());

            d.lastWidth_ = rect.width();
            setGeometry(rect);
//            repaint();
            return;
        }
    }

    if (!d.pressSizeGrip_)
        QMenu::mouseMoveEvent(event);
}

void OfficePopupMenu::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(OfficePopupMenu);
    d.pressSizeGrip_ = false;

    if (QWidget* w = d.findWidget(qtn_WidgetGallery))
    {
        if (w && !w->isWindow() && !w->isHidden())
            w->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }
    QMenu::mouseReleaseEvent(event);
}

void OfficePopupMenu::moveEvent(QMoveEvent* event)
{
    Q_UNUSED(event);
    QTN_D(OfficePopupMenu);
    if (d.pressSizeGrip_)
        setCursor(Qt::SizeFDiagCursor);
}

void OfficePopupMenu::resizeEvent(QResizeEvent* event)
{
    QMenu::resizeEvent(event);
}
