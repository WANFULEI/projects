/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonSystemPopupBar for Qt)
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
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QFileInfo>
#include <QColor>
#include <QKeySequence>
#include <qevent.h>
#include <qdebug.h>

#include "QtnRibbonSystemPopupBar.h"
#include "QtnRibbonBackstageView.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonButton.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonStyle.h"

using namespace Qtitan;


namespace Qtitan
{
    /* RibbonSystemPopupBarPrivate */
    class RibbonSystemButtonPrivate : QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonSystemButton)

    public:
        explicit RibbonSystemButtonPrivate() { widget_ = Q_NULL; }

    public:
        QWidget* widget_;
    };
};


/* RibbonSystemButton */
RibbonSystemButton::RibbonSystemButton(QWidget* parent)
    : QToolButton(parent)
{
    QTN_INIT_PRIVATE(RibbonSystemButton);
}

RibbonSystemButton::~RibbonSystemButton()
{
    QTN_FINI_PRIVATE();
}

void RibbonSystemButton::setBackstage(RibbonBackstageView* backstage)
{
    Q_ASSERT(backstage != Q_NULL);
    if (!backstage)
        return; 

    QTN_D(RibbonSystemButton);
    if (QAction* action = defaultAction())
    {
        d.widget_ = Q_NULL;
        if (QMenu* menu = action->menu())
        {
            action->setMenu(Q_NULL);
            delete menu;
        }

        d.widget_ = backstage;
        connect(action, SIGNAL(triggered()), backstage, SLOT(open()));
    }
}

RibbonBackstageView* RibbonSystemButton::backstage() const
{
    QTN_D(const RibbonSystemButton);
    return qobject_cast<RibbonBackstageView*>(d.widget_);
}

QSize RibbonSystemButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();

    const int heightTabs = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonTabsHeight, 0, 0) - 2;
    return QSize(sz.width() + 24, heightTabs).expandedTo(QApplication::globalStrut());
}

void RibbonSystemButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);

#ifdef Q_OS_WIN
    Qt::ToolButtonStyle styleButton = toolButtonStyle();
#endif // Q_OS_WIN

    QStyleOptionToolButton opt;
    initStyleOption(&opt);

#ifdef Q_OS_WIN
    opt.toolButtonStyle = styleButton;
#endif // Q_OS_WIN

    QRect rc = opt.rect;
    if (opt.toolButtonStyle == Qt::ToolButtonFollowStyle)
        rc.adjust(2, 2, -2, -2);

    int actualArea = rc.height()*rc.width();

    int index = -1;
    int curArea = 0;
    QList<QSize> lSz = opt.icon.availableSizes();
    for (int i = 0, count = lSz.count(); count > i; i++)
    {
        QSize curSz = lSz[i];
        int area = curSz.height()*curSz.width();
        if (actualArea > area)
        {
            if (area > curArea)
                index = i;
            curArea = area;
        }
    }

    opt.iconSize = index != -1 ?  opt.icon.actualSize(lSz[index]) : opt.icon.actualSize(opt.rect.adjusted(0, 2, 0, -2).size());

    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonFileButton, &opt, &p, this);

    // draw text
    if (opt.toolButtonStyle != Qt::ToolButtonFollowStyle && opt.toolButtonStyle != Qt::ToolButtonIconOnly)
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonFileButtonLabel, &opt, &p, this);
}

void RibbonSystemButton::mousePressEvent(QMouseEvent* event)
{
    QTN_D(RibbonSystemButton);
    if (RibbonBackstageView* backstage = qobject_cast<RibbonBackstageView*>(d.widget_))
    {
        if (backstage->isVisible())
        {
            backstage->close();
            return; 
        }
    }
    QToolButton::mousePressEvent(event);
}


namespace Qtitan
{
    /* RibbonSystemPopupBar */
    class SysPopupRibbonButton : public QToolButton
    {
    public:
        SysPopupRibbonButton(QWidget* parent);
        virtual ~SysPopupRibbonButton();

    protected:
        virtual bool event(QEvent* event);
        virtual void paintEvent(QPaintEvent* event);

    private:
        Q_DISABLE_COPY(SysPopupRibbonButton);
    };
};

SysPopupRibbonButton::SysPopupRibbonButton(QWidget* parent)
    : QToolButton(parent)
{
}

SysPopupRibbonButton::~SysPopupRibbonButton()
{
}

bool SysPopupRibbonButton::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
        event->setAccepted(false);
    return QToolButton::event(event);
}

void SysPopupRibbonButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    {
        QPainter p(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
        style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonPopupBarButton, &opt, &p, this);
    } //QToolButton::paintEvent() will create the second painter, so we need to destroy the first painter.
    QToolButton::paintEvent(event);
}

namespace Qtitan
{
    /* RibbonSystemPopupBarPrivate */
    class RibbonSystemPopupBarPrivate : QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonSystemPopupBar)

    public:
        explicit RibbonSystemPopupBarPrivate();
    public:
        QRect rcBorders_;
        QList<SysPopupRibbonButton*>  systemButtonList_;
        QList<QWidget*>  pageList_;
    };
};

RibbonSystemPopupBarPrivate::RibbonSystemPopupBarPrivate()
{
    rcBorders_ = QRect(QPoint(6, 18), QPoint(6, 29));
}


/* RibbonSystemPopupBar */
RibbonSystemPopupBar::RibbonSystemPopupBar(QWidget* parent)
    : QMenu(parent)
{
    QTN_INIT_PRIVATE(RibbonSystemPopupBar);
}

RibbonSystemPopupBar::~RibbonSystemPopupBar()
{
    QTN_FINI_PRIVATE();
}

QAction* RibbonSystemPopupBar::addPopupBarAction(const QString& text)
{
    QTN_D(RibbonSystemPopupBar);
    SysPopupRibbonButton* systemPopupBarButton = new SysPopupRibbonButton(this);
    systemPopupBarButton->setAutoRaise(true);
    systemPopupBarButton->setToolButtonStyle(Qt::ToolButtonTextOnly);

    d.systemButtonList_.append(systemPopupBarButton);
    systemPopupBarButton->setText(text);

    QAction* pAction = new QAction(text, systemPopupBarButton);
    systemPopupBarButton->setDefaultAction(pAction);
    return pAction;
}

void RibbonSystemPopupBar::addPopupBarAction(QAction* pAction, Qt::ToolButtonStyle style)
{
    QTN_D(RibbonSystemPopupBar);
    if (style == Qt::ToolButtonTextUnderIcon)
    {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid style button");
        return;
    }

    SysPopupRibbonButton* systemPopupBarButton = new SysPopupRibbonButton(this);
    systemPopupBarButton->setAutoRaise(true);
    systemPopupBarButton->setToolButtonStyle(style);

    d.systemButtonList_.append(systemPopupBarButton);
    systemPopupBarButton->setText(pAction->text());

    systemPopupBarButton->setDefaultAction(pAction);
}

RibbonPageSystemRecentFileList* RibbonSystemPopupBar::addPageRecentFile(const QString& caption)
{
    RibbonPageSystemRecentFileList* recentFileList = new RibbonPageSystemRecentFileList(caption);
    QTN_D(RibbonSystemPopupBar);
    d.pageList_.append(recentFileList);
    recentFileList->setParent(this);
    adjustSize();
    return recentFileList;
}

QSize RibbonSystemPopupBar::sizeHint() const
{
    QTN_D(const RibbonSystemPopupBar);

    QSize sz = QMenu::sizeHint();
    sz.setHeight(sz.height() + 12);

    if (d.pageList_.size() > 0)
        sz.setWidth(sz.width() + d.pageList_[0]->sizeHint().width() + d.rcBorders_.right()/2 - 1);
    return sz;
}

void RibbonSystemPopupBar::showEvent(QShowEvent* event)
{
    QMenu::showEvent(event);

    if (RibbonBar* parent = qobject_cast<RibbonBar*>(parentWidget()))
    {
        if (QToolButton* button = parent->getSystemButton())
        {
            if (button->toolButtonStyle() == Qt::ToolButtonFollowStyle)
            {
                QRect rectButton =  button->rect();
                if (rectButton.isValid())
                {
                    QPoint posButton = button->mapToGlobal(rectButton.topLeft());
                    QRect rectMenu = rect();
                    if (rectMenu.isValid())
                    {
                        QPoint posMenu = mapToGlobal(rectMenu.topLeft());
                        if (posButton.y() < posMenu.y())
                        {
                            // set new menu position
                            QPoint posParent = parent->mapToGlobal(parent->pos());
                            move(QPoint(posMenu.x(), posParent.y() + parent->titleBarHeight() + parent->topBorder()));
                        }
                    }
                }
            }
        }
    }
}

void RibbonSystemPopupBar::mousePressEvent(QMouseEvent* event)
{
    if (RibbonBar* parent = qobject_cast<RibbonBar*>(parentWidget()))
    {
        if (QToolButton* button = parent->getSystemButton())
        {
            if (button->toolButtonStyle() == Qt::ToolButtonFollowStyle)
            {
                QRect rectButton =  button->rect();
                if (rectButton.isValid())
                {
                    QPoint posButton = button->mapToGlobal(rectButton.topLeft());
                    if (QRect(posButton, button->size()).contains(event->globalPos()))
                    {
                        QMouseEvent ev(QEvent::MouseButtonPress, QPoint(-1, -1), event->button(), event->buttons(), event->modifiers());
                        QMenu::mousePressEvent(&ev);
                        return;
                    }
                }
            }
        }
    }
    QMenu::mousePressEvent(event);
}

void RibbonSystemPopupBar::paintEvent(QPaintEvent* event)
{
    QTN_D(RibbonSystemPopupBar);

    QPainter p(this);

    QRegion emptyArea = QRegion(rect());
    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
    menuOpt.tabWidth = 0;
    style()->drawPrimitive( QStyle::PE_FrameMenu, &menuOpt, &p, this );
    
    QList<QAction*> actionList = actions();
    for (int i = 0; i < actionList.count(); ++i) 
    {
        QAction *action = actionList.at( i );
        QRect adjustedActionRect = actionGeometry(action);

        if (!event->rect().intersects(adjustedActionRect))
            continue;

        QStyleOptionMenuItem opt;
        initStyleOption( &opt, action );
        opt.rect = adjustedActionRect;
        style()->drawControl( QStyle::CE_MenuItem, &opt, &p, this );
    }

    int hMargin = style()->pixelMetric(QStyle::PM_MenuHMargin, Q_NULL, this)*2;
    int count = d.pageList_.size();
    if (count > 0)
    {
        QSize sz = QMenu::sizeHint();
        StyleHintReturnThemeColor hintReturn(tr("Ribbon"), tr("MenuPopupSeparator"), QColor(197, 197, 197));
        const QColor fillColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturn));
        p.fillRect(QRect(QPoint(sz.width() + d.rcBorders_.left() - hMargin, d.rcBorders_.top()), 
            QSize(1, sz.height() - d.rcBorders_.top() - d.rcBorders_.bottom()/2-2)), fillColor);
    }

    for (int i = 0; i < count; i++)
    {
        if (RibbonPageSystemPopupBarListCaption* page = qobject_cast<RibbonPageSystemPopupBarListCaption*>(d.pageList_.at(i)))
        {
            QSize sz = QMenu::sizeHint();

            StyleHintReturnThemeColor hintReturn(tr("Ribbon"), tr("RecentFileListBackground"));
            const QColor fillColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturn));
            p.fillRect(page->geometry(), fillColor);
            break;
        }
    }

    if (RibbonBar* parent = qobject_cast<RibbonBar*>(parentWidget()))
    {
        if (QToolButton* button = parent->getSystemButton())
        {
            if (button->toolButtonStyle() == Qt::ToolButtonFollowStyle)
            {
                QStyleOptionToolButton opt;
                opt.state |= QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_Sunken;
                opt.activeSubControls |= QStyle::SC_ToolButtonMenu;
                opt.toolButtonStyle = Qt::ToolButtonFollowStyle;
                QRect rc = button->rect();
                QPoint posButton = button->mapToGlobal(rc.topLeft());
                posButton = mapFromGlobal(posButton);
                rc.translate(posButton);
                opt.rect = rc;
                opt.icon = button->icon();
                opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
                style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonFileButton, &opt, &p, this);
            }
        }
    }
}

void RibbonSystemPopupBar::keyPressEvent(QKeyEvent* event)
{
    QMenu::keyPressEvent(event);

    if (!event->isAccepted())
    {
        QTN_D(RibbonSystemPopupBar);
        for (int i = 0; i < d.pageList_.count(); i++)
        {
            if (RibbonPageSystemPopupBarListCaption* page = qobject_cast<RibbonPageSystemPopupBarListCaption*>(d.pageList_.at(i)))
            {
                page->keyPressEvent(event);
                break;
            }
        }
    }
}

void RibbonSystemPopupBar::resizeEvent(QResizeEvent* event)
{
    QMenu::resizeEvent(event);

    QTN_D(RibbonSystemPopupBar);

    QSize sz(rect().size());

    int nRight = sz.width() - d.rcBorders_.right() + 1;

    for (int i = 0; i < d.systemButtonList_.count(); ++i) 
    {
        SysPopupRibbonButton* barButton = d.systemButtonList_.at(i);

        QSize szControl = barButton->sizeHint();
        QRect rc( QPoint(nRight - szControl.width(), sz.height() - d.rcBorders_.bottom() + 4), QPoint(nRight, sz.height() - 3));
        barButton->move( rc.left(), rc.top() );
        barButton->resize( rc.width(), rc.height() );

        nRight -= szControl.width() + 6;
    }

    for (int i = 0; i < d.pageList_.size(); i++)
    {
        if (RibbonPageSystemPopupBarListCaption* page = qobject_cast<RibbonPageSystemPopupBarListCaption*>(d.pageList_.at(i)))
        {
            QSize sz = QMenu::sizeHint();
            int hMargin = style()->pixelMetric(QStyle::PM_MenuHMargin, Q_NULL, this)*2;
            QRect rc(QPoint(sz.width() + d.rcBorders_.left() - (hMargin-1), d.rcBorders_.top()), 
                QSize(page->sizeHint().width(), sz.height() - d.rcBorders_.bottom() - d.rcBorders_.top()/2+2));
            page->setGeometry(rc);
            break;
        }
    }
}



/* RibbonPageSystemPopupBarListCaption */
RibbonPageSystemPopupBarListCaption::RibbonPageSystemPopupBarListCaption(const QString& caption)
{
    setWindowTitle(caption);
}

RibbonPageSystemPopupBarListCaption::~RibbonPageSystemPopupBarListCaption()
{
}

QSize RibbonPageSystemPopupBarListCaption::sizeHint() const
{
    return QSize(300, 27);
}

void RibbonPageSystemPopupBarListCaption::drawEvent(QPaintEvent* event, QPainter* p)
{
    Q_UNUSED(event);
    Q_UNUSED(p);
}

void RibbonPageSystemPopupBarListCaption::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);

    QRect rc = opt.rect;
    QRect rcText(rc.left() + 7, rc.top() + 4, rc.right(), rc.bottom() - 5);

    QFont fontSave = p.font();

    QFont font = fontSave;
    font.setBold(true);
    p.setFont(font);

    StyleHintReturnThemeColor hintClientText(tr("Ribbon"), tr("GroupClientText"));
    const QColor colorClientText = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintClientText));
    opt.palette.setColor(QPalette::WindowText, colorClientText);
    uint alignment = Qt::AlignTop | Qt::TextSingleLine;
    style()->drawItemText(&p, rcText, alignment, opt.palette, true, windowTitle(), QPalette::WindowText);

    p.setFont(fontSave);

    StyleHintReturnThemeColor hintEdgeShadow(tr("Ribbon"), tr("RecentFileListEdgeShadow"));
    const QColor fillEdgeShadow = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintEdgeShadow));

    p.fillRect(QRect(QPoint(rc.left(), rc.top() + sizeHint().height() - 6), QSize(rc.width(), 1)), fillEdgeShadow);

    StyleHintReturnThemeColor hintEdgeHighLight(tr("Ribbon"), tr("RecentFileListEdgeHighLight"));
    const QColor edgeHighLightColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintEdgeHighLight));
    p.fillRect(QRect(QPoint(rc.left(), rc.top() + sizeHint().height() - 5 ), QSize(rc.width(), 1)), edgeHighLightColor);

    drawEvent(event, &p);
}


namespace Qtitan
{
    /* RibbonPageSystemRecentFileListPrivate */
    class RibbonPageSystemRecentFileListPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonPageSystemRecentFileList)
    public:
        explicit RibbonPageSystemRecentFileListPrivate();

    public:
        void updateActionRects() const;
        void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;
        QAction* actionAt(const QPoint& p) const;
        QRect actionRect(QAction* act) const;
        void setCurrentAction(QAction* currentAction);
        void activateAction(QAction* action, QAction::ActionEvent action_e, bool self = true);

    public:
        bool itemsDirty_;
        bool mouseDown_;
        QAction* currentAction_;
        QList<QAction*> recentFileActs_; // contents of the MRU list 

        mutable bool hasCheckableItems_;
        mutable QVector<QRect> actionRects_;
    };
};

RibbonPageSystemRecentFileListPrivate::RibbonPageSystemRecentFileListPrivate()
{
    itemsDirty_ = true;
    mouseDown_ = false;
    hasCheckableItems_ = false;
    currentAction_ = Q_NULL;
}

QAction* RibbonPageSystemRecentFileListPrivate::actionAt(const QPoint& pt) const
{
    QTN_P(const RibbonPageSystemRecentFileList);

    if (!p.rect().contains(pt)) //sanity check
        return 0;

    for(int i = 0; i < actionRects_.count(); i++)
    {
        if (actionRects_.at(i).contains(pt))
            return recentFileActs_.at(i);
    }
    return 0;
}

QRect RibbonPageSystemRecentFileListPrivate::actionRect(QAction* act) const
{
    int index = recentFileActs_.indexOf(act);
    if (index == -1)
        return QRect();

    updateActionRects();

    //we found the action
    return actionRects_.at(index);
}

void RibbonPageSystemRecentFileListPrivate::setCurrentAction(QAction* currentAction)
{
    QTN_P(RibbonPageSystemRecentFileList);

    if (currentAction_ && currentAction_ != currentAction)
        p.update(actionRect(currentAction_));

    if (currentAction_ == currentAction)
        return;
    currentAction_ = currentAction;

    p.update(actionRect(currentAction_));
}

void RibbonPageSystemRecentFileListPrivate::activateAction(QAction* action, QAction::ActionEvent action_e, bool self)
{
    Q_UNUSED(self);
    Q_UNUSED(action_e);

    QTN_P(RibbonPageSystemRecentFileList);
    p.parentWidget()->hide();

//    action->activate(action_e);
    emit p.openRecentFile(action->data().toString());
}

void RibbonPageSystemRecentFileListPrivate::updateActionRects() const
{
    QTN_P(const RibbonPageSystemRecentFileList);
    if (!itemsDirty_)
        return;

    p.ensurePolished();

    actionRects_.resize(recentFileActs_.count());
    actionRects_.fill(QRect());

    //let's try to get the last visible action
    int lastVisibleAction = recentFileActs_.count() - 1;
    for(;lastVisibleAction >= 0; --lastVisibleAction) 
    {
        const QAction *action = recentFileActs_.at(lastVisibleAction);
        if (action->isVisible())
            break;
    }

    QStyle* style = p.style();
    QStyleOption opt;
    opt.init(&p);

    const int hmargin = style->pixelMetric(QStyle::PM_MenuHMargin, &opt, &p),
        vmargin = style->pixelMetric(QStyle::PM_MenuVMargin, &opt, &p);

    const int fw = style->pixelMetric(QStyle::PM_MenuPanelWidth, &opt, &p);
    int max_column_width = p.sizeHint().width() - vmargin - fw; 
    int y = p.sizeHint().height();

    //calculate size
    QFontMetrics qfm = p.fontMetrics();
    for(int i = 0; i <= lastVisibleAction; i++) 
    {
        QAction *action = recentFileActs_.at(i);

        if (!action->isVisible())
            continue; // we continue, this action will get an empty QRect

        //let the style modify the above size..
        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        const QFontMetrics& fm = opt.fontMetrics;

        QSize sz;
        QString s = action->text();
        sz.setWidth(fm.boundingRect(QRect(), Qt::TextSingleLine | Qt::TextShowMnemonic, s).width());
        sz.setHeight(qMax(fm.height(), qfm.height()));
        sz = style->sizeFromContents(QStyle::CT_MenuItem, &opt, sz, &p);
        //update the item
        if (!sz.isEmpty()) 
            actionRects_[i] = QRect(0, 0, sz.width(), sz.height());
    }

    int x = hmargin + fw;
    for(int i = 0; i < recentFileActs_.count(); i++) 
    {
        QRect& rect = actionRects_[i];
        if (rect.isNull())
            continue;

        rect.translate(x, y); //move
        rect.setWidth(max_column_width); //uniform width

        y += rect.height();
    }
    
    ((RibbonPageSystemRecentFileListPrivate*)this)->itemsDirty_ = false;
}

void RibbonPageSystemRecentFileListPrivate::initStyleOption(QStyleOptionMenuItem* option, const QAction* action) const
{
    if (!option || !action)
        return;

    QTN_P(const RibbonPageSystemRecentFileList);

    option->initFrom(&p);
    option->palette = p.palette();
    option->state = QStyle::State_None;


    if (p.isEnabled() && action->isEnabled())
        option->state |= QStyle::State_Enabled;
    else
        option->palette.setCurrentColorGroup(QPalette::Disabled);

    option->font = action->font().resolve(p.font());
    option->fontMetrics = QFontMetrics(option->font);

    if (currentAction_ && currentAction_ == action) 
        option->state |= QStyle::State_Selected | (mouseDown_ ? QStyle::State_Sunken : QStyle::State_None);

    option->menuHasCheckableItems = hasCheckableItems_;
    if (!action->isCheckable()) 
    {
        option->checkType = QStyleOptionMenuItem::NotCheckable;
    } 
    else 
    {
        option->checkType = (action->actionGroup() && action->actionGroup()->isExclusive())
            ? QStyleOptionMenuItem::Exclusive : QStyleOptionMenuItem::NonExclusive;
        option->checked = action->isChecked();
    }

    option->menuItemType = QStyleOptionMenuItem::Normal;
    if (action->isIconVisibleInMenu())
        option->icon = action->icon();

    QString textAndAccel = action->text();

    if (textAndAccel.indexOf(QLatin1Char('\t')) == -1) 
    {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty())
            textAndAccel += QLatin1Char('\t') + QString(seq);
    }

    option->text = textAndAccel;
    option->tabWidth = 0;;
    option->maxIconWidth = 0;;
    option->menuRect = p.rect();
}

/* RibbonPageSystemRecentFileList */
RibbonPageSystemRecentFileList::RibbonPageSystemRecentFileList(const QString& caption)
    : RibbonPageSystemPopupBarListCaption(caption)
{
    QTN_INIT_PRIVATE(RibbonPageSystemRecentFileList);

    if (caption.isEmpty())
        setWindowTitle(tr("Recent Documents"));

    setAttribute(Qt::WA_MouseTracking, true);
}

RibbonPageSystemRecentFileList::~RibbonPageSystemRecentFileList()
{
    QTN_FINI_PRIVATE();
}

void RibbonPageSystemRecentFileList::setSize(int size)
{
    QTN_D(RibbonPageSystemRecentFileList);

    if (size < 0 || size > 9)
    {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Number of files could not be more than 9.");
        return; 
    }
    // clean actions
    for (int i = 0, count = d.recentFileActs_.count(); count > i; i++)
    {
        QAction* act = d.recentFileActs_[i];
        removeAction(act);
        delete act;
    }
    d.recentFileActs_.clear();

    for (int i = 0; i < size; ++i) 
    {
        QAction* recentFileAct = new QAction(this);
        recentFileAct->setVisible(false);
        addAction(recentFileAct);
        d.recentFileActs_.append(recentFileAct);
//        connect(recentFileAct, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
}

int RibbonPageSystemRecentFileList::getSize() const
{
    QTN_D(const RibbonPageSystemRecentFileList);
    return d.recentFileActs_.size();
}

QAction* RibbonPageSystemRecentFileList::getCurrentAction() const
{
    QTN_D(const RibbonPageSystemRecentFileList);
    return d.currentAction_;
}

static QString strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void RibbonPageSystemRecentFileList::updateRecentFileActions(const QStringList& files)
{
    QTN_D(RibbonPageSystemRecentFileList);

    if (d.recentFileActs_.size() == 0)
    {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Number of files not defined.");
        return;
    }

    int numRecentFiles = qMin(files.size(), d.recentFileActs_.size());

    for (int i = 0; i < numRecentFiles; ++i) 
    {
        QAction* recentFileAct = d.recentFileActs_.at(i);
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileAct->setText(text);
        recentFileAct->setData(files[i]);
        recentFileAct->setVisible(true);
    }

    for (int j = numRecentFiles; j < d.recentFileActs_.size(); ++j)
        d.recentFileActs_[j]->setVisible(false);

    d.itemsDirty_ = true;
    d.updateActionRects();
}


void RibbonPageSystemRecentFileList::drawEvent(QPaintEvent* event, QPainter* p)
{
    QTN_D(RibbonPageSystemRecentFileList);

    QRegion emptyArea = QRegion(rect());

    //draw the items that need updating..
    for (int i = 0; i < d.recentFileActs_.count(); ++i) 
    {
        QAction* action = d.recentFileActs_.at(i);
        QRect adjustedActionRect = d.actionRects_.at(i);
        if (!event->rect().intersects(adjustedActionRect)/* || d->widgetItems.value(action)*/)
            continue;
        //set the clip region to be extra safe (and adjust for the scrollers)
        QRegion adjustedActionReg(adjustedActionRect);
        emptyArea -= adjustedActionReg;
        p->setClipRegion(adjustedActionReg);

        QStyleOptionMenuItem opt;
        d.initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;
        QString text = opt.text;
        opt.text = QString();
        style()->drawControl(QStyle::CE_MenuItem, &opt, p, this);
        
        uint alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine | Qt::TextShowMnemonic;
        opt.rect.translate(7, 0); //move

        StyleHintReturnThemeColor hintClientTextReturn(tr("Ribbon"), tr("GroupClientText"));
        const QColor clientTextColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintClientTextReturn));
        opt.palette.setColor(QPalette::WindowText, clientTextColor);

        opt.text = p->fontMetrics().elidedText(text, Qt::ElideRight, opt.rect.adjusted(2, 0, -2, 0).width());
        style()->drawItemText(p, opt.rect, alignment, opt.palette, true, opt.text, QPalette::WindowText);
    }
}

void RibbonPageSystemRecentFileList::mousePressEvent(QMouseEvent* event)
{
//    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    RibbonPageSystemPopupBarListCaption::mousePressEvent(event);
}

void RibbonPageSystemRecentFileList::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(RibbonPageSystemRecentFileList);

    if (!isVisible())
        return;
    d.setCurrentAction(d.actionAt(event->pos()));
}

void RibbonPageSystemRecentFileList::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(RibbonPageSystemRecentFileList);

    QAction* action = d.actionAt(event->pos());
    if (action && action == d.currentAction_) 
    {
#if defined(Q_WS_WIN)
        //On Windows only context menus can be activated with the right button
        if (event->button() == Qt::LeftButton)
#endif
            d.activateAction(action, QAction::Trigger);
    }
    else
        RibbonPageSystemPopupBarListCaption::mouseReleaseEvent(event);
}

void RibbonPageSystemRecentFileList::enterEvent(QEvent* event)
{
    RibbonPageSystemPopupBarListCaption::enterEvent(event);

    QEvent leaveEvent(QEvent::Leave);
    QApplication::sendEvent(parentWidget(), &leaveEvent);

    setFocus();
}

void RibbonPageSystemRecentFileList::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);

    QTN_D(RibbonPageSystemRecentFileList);
    d.setCurrentAction(Q_NULL);
    parentWidget()->setFocus();
}

void RibbonPageSystemRecentFileList::keyPressEvent(QKeyEvent* event)
{
    QTN_D(RibbonPageSystemRecentFileList);

    int key = event->key();
    bool key_consumed = false;
    switch(key) 
    {
        case Qt::Key_Up:
        case Qt::Key_Down: 
            {
                key_consumed = true;
                QAction* nextAction = Q_NULL;
                if (!d.currentAction_) 
                {
                    if(key == Qt::Key_Down) 
                    {
                        for(int i = 0; i < d.recentFileActs_.count(); ++i) 
                        {
                            QAction* act = d.recentFileActs_.at(i);
                            if (d.actionRects_.at(i).isNull())
                                continue;
                            if ( (style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this) || act->isEnabled())) 
                            {
                                nextAction = act;
                                break;
                            }
                        }
                    } 
                    else 
                    {
                        for (int i = d.recentFileActs_.count()-1; i >= 0; --i) 
                        {
                            QAction* act = d.recentFileActs_.at(i);
                            if (d.actionRects_.at(i).isNull())
                                continue;
                            if ( (style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this) || act->isEnabled())) 
                            {
                                nextAction = act;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    for(int i = 0, y = 0; !nextAction && i < d.recentFileActs_.count(); i++) 
                    {
                        QAction *act = d.recentFileActs_.at(i);
                        if (act == d.currentAction_) 
                        {
                            if (key == Qt::Key_Up) 
                            {
                                for(int next_i = i-1; true; next_i--) 
                                {
                                    if (next_i == -1) 
                                    {
                                        if(!style()->styleHint(QStyle::SH_Menu_SelectionWrap, 0, this))
                                            break;
                                        next_i = d.actionRects_.count()-1;
                                    }
                                    QAction *next = d.recentFileActs_.at(next_i);
                                    if (next == d.currentAction_)
                                        break;
                                    if (d.actionRects_.at(next_i).isNull())
                                        continue;
                                    if (next->isSeparator() ||
                                        (!next->isEnabled() &&
                                        !style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this)))
                                        continue;
                                    nextAction = next;
                                    break;
                                }
                            } 
                            else 
                            {
                                y += d.actionRects_.at(i).height();
                                for(int next_i = i+1; true; next_i++) 
                                {
                                    if (next_i == d.recentFileActs_.count()) 
                                    {
                                        if(!style()->styleHint(QStyle::SH_Menu_SelectionWrap, 0, this))
                                            break;
                                        next_i = 0;
                                    }
                                    QAction* next = d.recentFileActs_.at(next_i);
                                    if (next == d.currentAction_)
                                        break;
                                    if (d.actionRects_.at(next_i).isNull())
                                        continue;
                                    if (!next->isEnabled() && !style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this))
                                        continue;
                                    nextAction = next;
                                    break;
                                }
                            }
                            break;
                        }
                        y += d.actionRects_.at(i).height();
                    }
                }

                if (nextAction) 
                    d.setCurrentAction(nextAction);
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter: 
            {
                if (!d.currentAction_) 
                {
                    key_consumed = true;
                    break;
                }
                d.activateAction(d.currentAction_, QAction::Trigger);
                key_consumed = true;
            }
            break;
        default:
            key_consumed = false;
            break;
    }

    if (!key_consumed)
    {
        if ((!event->modifiers() || event->modifiers() == Qt::AltModifier || event->modifiers() == Qt::ShiftModifier) && event->text().length()==1 ) 
        {
            bool activateAction = false;
            QAction *nextAction = 0;

            int clashCount = 0;
            QAction* first = 0, *currentSelected = 0, *firstAfterCurrent = 0;
            QChar c = event->text().at(0).toUpper();
            for(int i = 0; i < d.recentFileActs_.size(); ++i) 
            {
                if (d.actionRects_.at(i).isNull())
                    continue;
                QAction *act = d.recentFileActs_.at(i);
                QKeySequence sequence = QKeySequence::mnemonic(act->text());
                int key = sequence[0] & 0xffff;
                if (key == c.unicode()) 
                {
                    clashCount++;
                    if (!first)
                        first = act;
                    if (act == d.currentAction_)
                        currentSelected = act;
                    else if (!firstAfterCurrent && currentSelected)
                        firstAfterCurrent = act;
                }
            }
            if (clashCount == 1)
                activateAction = true;

            if (clashCount >= 1) 
            {
                if (clashCount == 1 || !currentSelected || !firstAfterCurrent)
                    nextAction = first;
                else
                    nextAction = firstAfterCurrent;
            }

            if (nextAction) 
            {
                key_consumed = true;
                if (!nextAction->menu() && activateAction)
                    d.activateAction(nextAction, QAction::Trigger);
            }
        }

#ifdef Q_OS_WIN32
        if (key_consumed && (event->key() == Qt::Key_Control || event->key() == Qt::Key_Shift || event->key() == Qt::Key_Meta))
            QApplication::beep();
#endif // Q_OS_WIN32
    }

    if (key_consumed)
        event->accept();
    else
        event->ignore();
}

void RibbonPageSystemRecentFileList::resizeEvent(QResizeEvent* event)
{
    RibbonPageSystemPopupBarListCaption::resizeEvent(event);

    QTN_D(RibbonPageSystemRecentFileList);
    d.updateActionRects();
}
