/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonBar for Qt)
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
#include <QDesktopWidget>
#include <QMainWindow>
#include <QStyleOption>
#include <QPainter>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QLineEdit>
#include <QWidgetAction>
#include <qevent.h>
#include <qdebug.h>

#include "QtnRibbonBar.h"
#include "QtnRibbonPrivate.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonButton.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonBarPrivate.h"
#include "QtnOfficeFrameHelper.h"
#include "QtnRibbonQuickAccessBar.h"
#include "QtnRibbonSystemPopupBar.h"
#include "QtnRibbonBackstageView.h"
#include "QtnRibbonToolTip.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonGallery.h"


#ifdef Q_OS_WIN
#include "QtnOfficeFrameHelper_win.h"
#endif // Q_OS_WIN

using namespace Qtitan;

/*!
\class Qtitan::RibbonBarPrivate
\internal
*/
RibbonBarPrivate::RibbonBarPrivate()
{
    tabBar_ = Q_NULL;
    quickAccessBar_ = Q_NULL;
    controlSystemButton_ = Q_NULL;
    currentIndexPage_ = -1;
    frameHelper_ = Q_NULL;
    showTitleAlways_ = true;
    quickAccessOnTop_ = true;
    ribbonBarVisible_ = true;
    minimizationEnabled_ = true;
    KeyTipsComplement_ = true;
    minimized_ = false;
    hoveredSubControl_ = QStyle::SC_None;
    activeSubControl_ = QStyle::SC_None;
    isActive_ = false;
    countKey_ = 0;
}

RibbonBarPrivate::~RibbonBarPrivate()
{
    removeAllContextHeaders();
}

void RibbonBarPrivate::init()
{
    QTN_P(RibbonBar);

    p.setNativeMenuBar(false);

    p.setAttribute(Qt::WA_Hover, true);
    p.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum); 
    
    tabBar_ = new RibbonTabBar(&p);
    tabBar_->show();
    QObject::connect(tabBar_, SIGNAL(currentChanged(int)), &p, SLOT(currentChanged(int)));

    quickAccessBar_ = new RibbonQuickAccessBar(&p);
    quickAccessBar_->setVisible(false);
}

void RibbonBarPrivate::layoutRibbon()
{
    QTN_P(RibbonBar);

    /* In MacOSX the font has been selected in RibbonStyle::polish(), so we have non valid reference to the tabbar. */
    if (tabBar_ == Q_NULL || quickAccessBar_ == Q_NULL)
        return;

    QWidget* parent = p.parentWidget();
    bool updates = parent ? parent->updatesEnabled() : true;
    if (parent && updates)
        parent->setUpdatesEnabled(false);

    QStyleOption opt;
    opt.init(&p);

    ribbonBarVisible_ = true;

    const int vmargin = p.style()->pixelMetric(QStyle::PM_MenuBarVMargin, &opt, &p);
//    const int heightGroup = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, &opt, 0);
    int heightGroup = getMaxHeightPages();
//    const int heightCaptionGroup = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, 0);
    const int tabsHeight = tabBar_->sizeHint().height();
    const int pageAreaHeight = heightGroup/* + heightCaptionGroup*/;

    int quickAccessHeight = quickAccessBar_->sizeHint().height();

    bool frameThemeEnabled = p.isFrameThemeEnabled();
    bool showTitle = frameThemeEnabled || showTitleAlways_;

    int frameBorder = 4;

    QRect rcTabBar(QPoint(2, 0), QSize(opt.rect.width()-2, tabsHeight));
    QRect rcPageArea(QPoint(0, tabsHeight), QSize(opt.rect.width(), pageAreaHeight + vmargin * 2 + 2 + 1/* * 4*/ /*+ vmargin + 1*/));

    if (showTitle)
    {
        frameBorder = frameHelper_ ? frameHelper_->getFrameBorder() : 0;
        int ribbonTopBorder = p.topBorder();
        int titleOffset = ribbonTopBorder - frameBorder;

        rcTitle_ = QRect(QPoint(-frameBorder, titleOffset), 
            QPoint(opt.rect.width() + frameBorder, p.titleBarHeight() + ribbonTopBorder));

        rcTabBar.translate(0, rcTitle_.height() + titleOffset);
        rcPageArea.translate(0, rcTitle_.height() + titleOffset);

        rcHeader_ = QRect(0, 0, opt.rect.width(), rcTitle_.bottom());
        rcTitleText_  = QRect(0, 0, opt.rect.width(), rcTitle_.bottom());

        if (!frameHelper_)
            rcTitleText_ = QRect();
    }
    else
    {
//        if (!showTitle && controlSystemButton_)
//        {
//            if (controlSystemButton_->toolButtonStyle() != Qt::ToolButtonTextOnly)
//                controlSystemButton_->setToolButtonStyle(Qt::ToolButtonTextOnly);
//        }

        rcTitle_ = QRect();
        rcHeader_ = QRect();
        rcTitleText_ = QRect();
    }

    if (controlSystemButton_ && controlSystemButton_->toolButtonStyle() == Qt::ToolButtonTextBesideIcon)
        controlSystemButton_->setToolButtonStyle(Qt::ToolButtonTextOnly);

    int corner = quickAccessHeight + tabsHeight - 3;

    if (!controlSystemButton_ || controlSystemButton_->toolButtonStyle() != Qt::ToolButtonFollowStyle)
    {
        StyleOptionTitleBar titleBarOpt;
        titleBarOptions(titleBarOpt);

        QSize szIcon = frameHelper_ ? frameHelper_->getSizeSystemIcon() : QSize(16, 16);
//        QSize szIcon = titleBarOpt.icon.actualSize(QSize(64, 64));
        corner = 10 + (!titleBarOpt.icon.isNull() ? /*titleBarOpt.icon.actualSize(QSize(16, 16)).width()*/szIcon.width() : 0);
    }

    QSize szBar = quickAccessBar_->sizeHint();
    if (quickAccessBar_->isVisible() && !quickAccessOnTop_)
        rcQuickAccess_ = QRect(QPoint(0, rcPageArea.bottom()), QPoint(szBar.width(), rcPageArea.bottom() + quickAccessHeight));
    else
    {
        rcQuickAccess_ = QRect(QPoint(5 + corner, rcTitle_.top() + frameBorder), szBar);
//        rcQuickAccess_ = QRect(QPoint(3 + corner, rcTitle_.top() + frameBorder - 2), 
//            QPoint(6 + corner + szBar.width(), rcTitle_.top() + /*frameBorder - 2 +*/ quickAccessHeight - 1));
    }

    QPoint posAccessBar = rcQuickAccess_.topLeft();
    quickAccessBar_->move(posAccessBar);
//    quickAccessBar_->resize(rcQuickAccess_.size());

    int nLeft = 2;
    int nRight = opt.rect.width() - 2;
    if (controlSystemButton_)
    {
        QSize szControl(corner + 1, corner + 1 /*- 1*/);
        int nTop = rcTitle_.top() + frameBorder - 1;
        nLeft = 0;
        int nGap = 0;

        if (controlSystemButton_->toolButtonStyle() != Qt::ToolButtonFollowStyle)
        {
            szControl = controlSystemButton_->sizeHint();
            szControl.setHeight(tabsHeight);

            nTop = rcTabBar.top() + (tabsHeight - szControl.height()) / 2;
            nGap = 3;
        }
        else if (!showTitle)
        {
            szControl = controlSystemButton_->sizeHint();
            nTop = rcTabBar.top() + (tabsHeight - szControl.height()) / 2;
        }

        QRect rcControl(QPoint(nLeft, nTop), QPoint(nLeft + szControl.width(), nTop + szControl.height()));
        nLeft += szControl.width() + nGap;
        controlSystemButton_->setGeometry(rcControl);
    }

    bool quickAccessNearTabs = quickAccessBar_->isVisible() && quickAccessOnTop_ && !showTitle;

    if (quickAccessNearTabs)
    {
//      nLeft += 5;
        rcQuickAccess_.setLeft(nLeft);
        rcQuickAccess_.setTop(rcTabBar_.top());
        rcQuickAccess_.setBottom(rcTabBar_.bottom());

        quickAccessBar_->move(rcQuickAccess_.topLeft());

        nLeft += szBar.width();
    }
    else
    {
//      nLeft = rcQuickAccess_.left() + (!quickAccessOnTop_ ? 2 : 0);
    }

    if (nLeft + 6 > nRight || !tabBar_->isVisible())
        rcTabBar_ = QRect(QPoint(0, rcTabBar.top()), QPoint(0, rcTabBar.bottom() + 1));
    else
        rcTabBar_ = QRect(QPoint(nLeft, rcTabBar.top()), QPoint(nRight, rcTabBar.bottom() + 1));

    tabBar_->setGeometry(rcTabBar_);
    tabBar_->layoutWidgets();

    rcPageArea_ = rcPageArea;

    if (minimized_)
        rcPageArea_.setHeight(0);

    int index = tabBar_->currentIndex();
    if (validIndex(index))
    {
        RibbonPage* page = listPages_.at(index);
        page->setGroupsHeight(heightGroup /*+ heightCaptionGroup + vmargin * 2*/);
        page->setGeometry(rcPageArea_);
    }

    layoutTitleButton();

    ribbonBarVisible_ = p.isTitleBarVisible() ? rcHeader_.width() > 3 : opt.rect.width() >= 250;

    if (frameHelper_)
        frameHelper_->setHeaderRect(rcHeader_);

    if (showTitle && ribbonBarVisible_)
        layoutContextHeaders();
    else
        removeAllContextHeaders();

    if (parent && updates)
        parent->setUpdatesEnabled(updates);
}

void RibbonBarPrivate::layoutTitleButton()
{
    QTN_P(RibbonBar);
    if (!p.isFrameThemeEnabled())
        return;

    StyleOptionTitleBar titleBarOpt;
    titleBarOptions(titleBarOpt);

    if (ribbonBarVisible_)
    {
        QRect rcQuickAccess = rcQuickAccess_;
        if (!quickAccessOnTop_ || !quickAccessBar_->isVisible())
        {
            if (!controlSystemButton_)
                rcHeader_.setLeft(rcHeader_.left() + /*30*/titleBarOpt.icon.actualSize(QSize(16, 16)).width() + 2);
            else
            {
                if (controlSystemButton_->toolButtonStyle() == Qt::ToolButtonFollowStyle)
                    rcHeader_.setLeft(rcHeader_.left() + /*70*/controlSystemButton_->sizeHint().width() - 5);
                else
                    rcHeader_.setLeft(rcHeader_.left() + /*30*/titleBarOpt.icon.actualSize(QSize(16, 16)).width() + 2);
            }

            if (controlSystemButton_)
                rcQuickAccess = controlSystemButton_->rect();
            else
                rcQuickAccess = QRect();
        }
        else
        {
            rcQuickAccess.setRight(rcQuickAccess.left() + (quickAccessBar_->isVisible() ? quickAccessBar_->width() + 2 : 12));
            rcHeader_.setLeft(rcQuickAccess.right());
        }
    }
    else
    {
        int corner = 2 + (!titleBarOpt.icon.isNull() ? titleBarOpt.icon.actualSize(QSize(16, 16)).width() : 0);
        rcHeader_.setLeft(corner);
    }

    bool dwmEnabled = frameHelper_->isDwmEnabled();

    int frameBorder = frameHelper_ && dwmEnabled ? frameHelper_->getFrameBorder() : 0;
    rcHeader_.adjust(0, frameBorder, 0, -3);

    QRect rcHeader = rcHeader_;

//    StyleOptionTitleBar titleBarOpt;
//    titleBarOptions(titleBarOpt);

    bool buttonClose = titleBarOpt.titleBarFlags & Qt::WindowSystemMenuHint;

    bool buttonMax =  titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint
        && !(titleBarOpt.titleBarState & Qt::WindowMaximized); 

    bool buttonMin =  titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint
        && !(titleBarOpt.titleBarState & Qt::WindowMinimized); 

    bool buttonNormal = (((titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint)
        && (titleBarOpt.titleBarState & Qt::WindowMinimized))
        || ((titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint)
        && (titleBarOpt.titleBarState & Qt::WindowMaximized)));

    actCloseButton_ = addTitleButton(QStyle::SC_TitleBarCloseButton, !dwmEnabled && buttonClose, rcHeader_);
    actNormalButton_ = addTitleButton(QStyle::SC_TitleBarNormalButton, !dwmEnabled && buttonNormal, rcHeader_);
    actMaxButton_ = addTitleButton(QStyle::SC_TitleBarMaxButton, !dwmEnabled && buttonMax, rcHeader_);
    actMinButton_ = addTitleButton(QStyle::SC_TitleBarMinButton, !dwmEnabled && buttonMin, rcHeader_);

    if (dwmEnabled)
        rcHeader_.setRight(rcHeader_.right() - 100);
//  else
//      rcHeader_.setRight(rcHeader_.right() - (rcHeader_.width() - rcHeader.width()));

    rcTitleText_ = QRect(rcTitleText_.left() + (rcHeader.right() - rcHeader_.right()), rcHeader_.top(), 
         rcHeader_.right() - (rcHeader.right() - rcHeader_.right()), rcHeader_.bottom() - rcHeader_.top() );

    QString strTitle = p.parentWidget() ? p.parentWidget()->windowTitle() : QString();

    const QFont font = QApplication::font("QWorkspaceTitleBar");
    const QFontMetrics fm(font);

    QRect br(fm.boundingRect(strTitle));

    int nCaptionLength = br.width()/* + 20*/;

    if (dwmEnabled)
        nCaptionLength += 2 * 10;// glowSize = 10 for Windows7 (Vista)

    nCaptionLength = qMin(rcTitleText_.width(), nCaptionLength);

    QRect rcTitleText(rcTitleText_);
    rcTitleText.setLeft(rcTitleText.left() + (rcTitleText.width() - nCaptionLength) / 2);
    if (rcTitleText.left() < rcHeader_.left())
        rcTitleText.setLeft(rcHeader_.left());

    rcTitleText.setRight(rcTitleText.left() + nCaptionLength);
/*
    if (rcQuickAccess.intersects(rcQuickAccess, rcCaptionText))
    {
        nCaptionLength = qMin(m_rcHeader.width(), nCaptionLength);
        rcCaptionText.setLeft(rcHeader_.left() + (rcHeader_.width() - nCaptionLength) / 2);
        rcCaptionText.setRight(rcCaptionText.left + nCaptionLength);
    }
*/
    if (rcTitleText.right() > rcHeader_.right()) 
        rcTitleText.setRight(rcHeader_.right());

    rcTitleText_ = rcTitleText;
}

void RibbonBarPrivate::layoutContextHeaders()
{
    QTN_P(RibbonBar);
    removeAllContextHeaders();

    ContextHeader* prevContextHeader = Q_NULL;

    if (!(p.isFrameThemeEnabled() || showTitleAlways_))
        return;

    int count = tabBar_->getTabCount();
    if (count == 0)
        return;

    if (quickAccessOnTop_ && quickAccessBar_->isVisible() && !frameHelper_)
        rcHeader_.setLeft(rcQuickAccess_.right() + (quickAccessBar_ ? quickAccessBar_->sizeHint().width() + 2 : 12));

    for (int i = 0; i < count; i++)
    {
        RibbonTab* tab = tabBar_->getTab(i);

        if (!tab->isVisible())
            continue;

        if (tab->getContextColor() == ContextColorNone || tab->contextTextTab().isEmpty())
        {
            prevContextHeader = Q_NULL;
            continue;
        }

        if (prevContextHeader && prevContextHeader->color == tab->getContextColor() &&
            prevContextHeader->strTitle == tab->contextTextTab())
        {
            prevContextHeader->rcRect.setRight(tab->rect().right());
            prevContextHeader->lastTab = tab;
            tab->setContextHeader(prevContextHeader);
        }
        else
        {
            ContextHeader* header = new ContextHeader(tab);
            QRect rect = tab->rect();
            QPoint topLeft = tab->mapToParent(rect.topLeft());
            QPoint bottomRight = tab->mapToParent(rect.bottomRight());

            topLeft = tabBar_->mapToParent(topLeft);
            bottomRight = tabBar_->mapToParent(bottomRight);

            header->rcRect = QRect(QPoint(topLeft.x(), rcTitle_.top() + 4), QPoint(bottomRight.x(), rcTitle_.bottom()));

            listContextHeaders_.append(header);
            prevContextHeader = header;
        }
    }

    count = listContextHeaders_.count();
    if (count == 0)
        return;

    if (!reduceContextHeaders())
    {
        removeAllContextHeaders();
        return;
    }

    QRect rcHeaders(QPoint(listContextHeaders_.at(0)->rcRect.left(), rcTitle_.top() + 6),
        QPoint(listContextHeaders_.at(count - 1)->rcRect.right(), rcTitle_.bottom()));

    int captionLength = rcTitleText_.width();
    QRect rcCaptionText(rcTitleText_);

    if (rcCaptionText.intersects(rcHeaders))
    {
        if (rcCaptionText.center().x() > rcHeaders.center().x())
        {
            if (rcHeader_.right() - rcHeaders.right() < captionLength)
            {
                rcTitleText_.setLeft(rcHeaders.right());
                rcTitleText_.setRight(rcHeader_.right());
            }
            else
            {
                rcTitleText_.setLeft(rcHeaders.right());
                rcTitleText_.setRight(rcHeaders.right() + captionLength);
            }
        }
        else
        {
            if (rcHeaders.left() - rcHeader_.left() < captionLength)
            {
                rcTitleText_.setLeft(rcHeader_.left());
                rcTitleText_.setRight(rcHeaders.left());
            }
            else
            {
                rcTitleText_.setLeft(rcHeaders.left() - captionLength);
                rcTitleText_.setRight(rcHeaders.left());
            }
        }
    }
}

void RibbonBarPrivate::layoutCorner()
{
    return;
    QTN_P(RibbonBar);
    if (QWidget* rightCorner = p.cornerWidget(Qt::TopRightCorner))
    {
        QRect rectCorner = rightCorner->geometry();
        QRect rectBar = tabBar_->geometry();
        int offset = (rectBar.height() - rectCorner.height())/2;
        rectCorner.moveTop(rectBar.top() + offset);
        rightCorner->move(rectCorner.topLeft());
    }
}

bool RibbonBarPrivate::reduceContextHeaders()
{
    int left = rcHeader_.left();
    int right = rcHeader_.right();

    int count = listContextHeaders_.count();

    for ( int i = 0; i < count; i++)
    {
        ContextHeader* header =  listContextHeaders_.at(i);
        if (header->rcRect.left() < left)
        {
            header->rcRect.setLeft(left);
            if (header->rcRect.width()  < 40)
                header->rcRect.setRight(left + 40);

            left = header->rcRect.right();
        }
        else break;
    }
    for (int i = count - 1; i >= 0; i--)
    {
        ContextHeader* header =  listContextHeaders_.at(i);
        if (header->rcRect.right() > right)
        {
            header->rcRect.setRight(right);
            if (header->rcRect.width() < 40)
                header->rcRect.setLeft(right - 40);

            right = header->rcRect.left();

            if (right < left)
                return false;
        }
        else break;
    }
    return true;
}

void RibbonBarPrivate::removeAllContextHeaders()
{
    for (int i = 0; i < listContextHeaders_.count(); i++)
        delete listContextHeaders_.at(i);
    listContextHeaders_.clear();
}

void RibbonBarPrivate::updateMinimizedModePage(RibbonPage* page)
{
    QTN_P(RibbonBar);
    if (minimized_)
    {
        page->setPageVisible(false);
        page->setWindowFlags(Qt::Popup);
        page->setFocusPolicy(Qt::NoFocus);
        page->setFocusProxy(&p);
        page->setRibbonMinimized(true);

        QEvent ev(QEvent::LayoutRequest);
        QApplication::sendEvent(page, &ev);
    }
    else
    {
        Qt::WindowFlags flags = page->windowFlags();
        if(flags & Qt::Popup) 
        {
            flags &= ~Qt::Popup;
            page->setWindowFlags(flags);
            page->setRibbonMinimized(false);

            QEvent ev(QEvent::LayoutRequest);
            QApplication::sendEvent(page, &ev);
        }
    }
}

void RibbonBarPrivate::swapVisiblePages(int index)
{
    if (validIndex(index))
    {
        for (int i = 0, count = listPages_.size(); count > i; i++)
            listPages_.at(i)->setPageVisible(index == i);
    }
}

QAction* RibbonBarPrivate::createSystemButton(const QIcon& icon, const QString& text)
{
    QTN_P(RibbonBar);
    QAction* actionSystemPopupBar = Q_NULL;
    if (icon.isNull())
    {
        QIcon iconQtnLogo;
        QIcon qtitan(":/res/qtitan.png");
        iconQtnLogo.addPixmap(qtitan.pixmap(QSize(64,56), QIcon::Disabled));
        QIcon icon32(":/res/qtitanlogo32x32.png");
        iconQtnLogo.addPixmap(icon32.pixmap(QSize(32,32), QIcon::Disabled));
        actionSystemPopupBar = new QAction(iconQtnLogo, text, &p);
    }
    else
        actionSystemPopupBar = new QAction(icon, text, &p);

    controlSystemButton_ = new RibbonSystemButton(&p);
    controlSystemButton_->show();
    controlSystemButton_->setAutoRaise(true);
    controlSystemButton_->setFocusPolicy(Qt::NoFocus);
    controlSystemButton_->setDefaultAction(actionSystemPopupBar);

    controlSystemButton_->setPopupMode(QToolButton::InstantPopup);
    controlSystemButton_->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    actionSystemPopupBar->setIconText(actionSystemPopupBar->text());

    return actionSystemPopupBar;
}

RibbonPage* RibbonBarPrivate::insertPage(int indTab, int index)
{
    QTN_P(RibbonBar);
    RibbonPage* page = new RibbonPage(&p);
    page->hide();
    insertPage(indTab, index, page);
    
    return page;
}

void RibbonBarPrivate::insertPage(int indTab, int index, RibbonPage* page)
{
    Q_ASSERT(page != Q_NULL);

    bool layoutPage = index == 0 && listPages_.size() == 0;
    if (!validIndex(index)) 
    {
        index = listPages_.count();
        listPages_.append(page);
    }
    else
        listPages_.insert(index, page);

    if (RibbonTab* tab = tabBar_->getTab(indTab))
        page->setAssociativeTab(tab);

    if (!minimized_)
    {
        int currentIndex = tabBar_->currentIndex();
        if (currentIndex != -1 && currentIndexPage_ != currentIndex &&
            currentIndex < listPages_.count())
        {
            swapVisiblePages(currentIndex);
            currentIndexPage_ = currentIndex;
        }
        if (layoutPage)
            swapVisiblePages(index);
    }
}

void RibbonBarPrivate::removePage(int index, bool deletePage)
{
    if (validIndex(index))
    {
        RibbonPage* page = listPages_[index];
        listPages_.removeAt(index);
        if (deletePage)
            delete page;
        else
        {
            updateMinimizedModePage(page);
            page->setAssociativeTab(Q_NULL);
            page->setParent(Q_NULL);
        }
    }
    currentIndexPage_ = -1;
}

int RibbonBarPrivate::getIndexPage(RibbonPage* page) const
{
    return listPages_.indexOf(page);
}

void RibbonBarPrivate::currentChanged(int index)
{
    QTN_P(RibbonBar);

    if (validIndex(index))
    {
        if (!minimized_) 
        {
            bool updates = p.updatesEnabled();
            if (updates)
                p.setUpdatesEnabled(false);

            emit p.currentPageChanged(index);
            emit p.currentPageChanged(listPages_[index]);
            swapVisiblePages(index);

            QEvent ev(QEvent::LayoutRequest);
            QApplication::sendEvent(p.getPage(index), &ev);

            layoutRibbon();

            if (updates)
                p.setUpdatesEnabled(updates);
        }
        else
        {
            if (RibbonPage* page = p.getPage(index))
            {
                page->setGeometry(QRect(QPoint(0,0), QSize(1, 1)));

                bool updates = page->updatesEnabled();
                if (updates)
                    page->setUpdatesEnabled(false);

                QWidget* activePopup = qobject_cast<RibbonPage*>(QApplication::activePopupWidget());

                QStyleOption opt;
                opt.init(page);

                const int vmargin = p.style()->pixelMetric(QStyle::PM_MenuBarVMargin, &opt, &p);
                int heightGroup = getMaxHeightPages();
                int height = heightGroup + vmargin * 2 + vmargin + 1;
                page->setGroupsHeight(heightGroup);

                QPoint pnt(0, 0);
                QRect rect = p.rect();
                rect.setTop(rcTabBar_.top());
                rect.setBottom(rcTabBar_.bottom());

                QRect screen = QApplication::desktop()->availableGeometry(&p);
                int h = height;
                if (p.mapToGlobal(QPoint(0, rect.bottom())).y() + h <= screen.height()) 
                    pnt = p.mapToGlobal(rect.bottomLeft());
                else 
                    pnt = p.mapToGlobal(rect.topLeft() - QPoint(0, h));

                page->popup();
                page->setGeometry(QRect(pnt, QSize(p.sizeHint().width(), height)));

                if (updates)
                    page->setUpdatesEnabled(updates);

                page->repaint();

                if (activePopup)
                    activePopup->close();
            }
        }
    }
}

void RibbonBarPrivate::titleBarOptions(StyleOptionTitleBar& titleBarOptions) const
{
    QTN_P(const RibbonBar);
    QWidget* widget = p.parentWidget();
    if (!widget)
        return;

    titleBarOptions.initFrom(widget);

    if (activeSubControl_ != QStyle::SC_None) 
    {
        if (hoveredSubControl_ == activeSubControl_) 
        {
            titleBarOptions.state |= QStyle::State_Sunken;
            titleBarOptions.activeSubControls = activeSubControl_;
        }
    }
    else if (p.style()->styleHint(QStyle::SH_TitleBar_AutoRaise, 0, widget) && 
             hoveredSubControl_ != QStyle::SC_None &&
             hoveredSubControl_ != QStyle::SC_TitleBarLabel)
    {
        titleBarOptions.state |= QStyle::State_MouseOver;
        titleBarOptions.activeSubControls = hoveredSubControl_;
    }
    else
    {
        titleBarOptions.state &= ~QStyle::State_MouseOver;
        titleBarOptions.activeSubControls = QStyle::SC_None;
    }

    titleBarOptions.subControls = QStyle::SC_All;
    titleBarOptions.titleBarFlags = widget->windowFlags();
    titleBarOptions.titleBarState = widget->windowState();

    if (titleBarOptions.titleBarFlags & Qt::WindowTitleHint)
    {
        QIcon icon = p.parentWidget() ? p.parentWidget()->windowIcon() : QIcon();
        QSize s = icon.actualSize(QSize(64, 64));
        titleBarOptions.icon = icon.pixmap(s);
    }

    if (frameHelper_ && titleBarOptions.icon.isNull())
        titleBarOptions.icon = frameHelper_->getFrameSmallIcon();

    if (controlSystemButton_ && controlSystemButton_->isVisible())
        titleBarOptions.drawIcon = !controlSystemButton_  || controlSystemButton_->toolButtonStyle() != Qt::ToolButtonFollowStyle;

    int border = p.style()->styleHint(QStyle::SH_TitleBar_NoBorder, &titleBarOptions, widget) ? 4 : 0;

    int paintHeight = p.titleBarHeight();
    paintHeight -= p.isMinimized() ? 2 * border : border;
    titleBarOptions.rect = rcTitle_;
    
    RibbonBarPrivate* that = const_cast<RibbonBarPrivate*>(this);
    that->windowTitle_ = p.parentWidget() ? p.parentWidget()->windowTitle() : QString();

    if (!windowTitle_.isEmpty()) 
    {
        // Set the text here before asking for the width of the title bar label
        // in case people uses the actual text to calculate the width.
        titleBarOptions.text = windowTitle_;
        const QFont font = QApplication::font("QWorkspaceTitleBar");
        titleBarOptions.fontMetrics = QFontMetrics(font);
        int width = rcHeader_.width();
        // Set elided text if we don't have enough space for the entire title.
        titleBarOptions.text = titleBarOptions.fontMetrics.elidedText(windowTitle_, Qt::ElideRight, width);
    }

    titleBarOptions.rcTitleText = rcTitleText_;
    titleBarOptions.quickAccessVisibleCount = quickAccessBar_ ? quickAccessBar_->getVisibleCount() : 0;
    titleBarOptions.quickAccessOnTop = quickAccessOnTop_;
    QSize szAccessBar =  quickAccessBar_->size();
    titleBarOptions.quickAccessVisible = quickAccessBar_ && quickAccessBar_->isVisible() && !szAccessBar.isNull();
    titleBarOptions.rcQuickAccess = rcQuickAccess_;
    titleBarOptions.existSysButton = controlSystemButton_;
    titleBarOptions.normalSysButton = controlSystemButton_ ? (controlSystemButton_->toolButtonStyle() != Qt::ToolButtonFollowStyle) : true;
}

QStyle::SubControl RibbonBarPrivate::getSubControl(const QPoint& pos) const
{
    QTN_P(const RibbonBar);
    StyleOptionTitleBar opt;
    titleBarOptions(opt);
    return p.style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt, pos, p.parentWidget());
}

RibbonTitleButton* RibbonBarPrivate::findTitleButton(QStyle::SubControl subControl) const
{
    QTN_P(const RibbonBar);
    QList<RibbonTitleButton*> buttons_list = qFindChildren<RibbonTitleButton*>(&p);
    foreach (RibbonTitleButton* but, buttons_list) 
    {
        if (but->getSubControl() == subControl)
            return but;
    }
    return Q_NULL;
}

QAction* RibbonBarPrivate::addTitleButton(QStyle::SubControl subControl, bool add, QRect& rcControl)
{
    QTN_P(RibbonBar);

    RibbonTitleButton* but = findTitleButton(subControl);

    QAction* act = but ? but->defaultAction() : Q_NULL;

    if (!but && add)
    {
        act = new QAction(&p);
        but = new RibbonTitleButton(&p, subControl);
        but->show();
        but->setDefaultAction(act);
        connect(act, SIGNAL(triggered()), this, SLOT(processClickedSubControl()));
        act->setToolTip(frameHelper_->getSystemMenuString(subControl));
    }
    else if (but&& !add)
    {
        act = but->defaultAction();
        disconnect(act, SIGNAL(triggered()), this, SLOT(processClickedSubControl()));

        delete but;
        but = Q_NULL;

        delete act;
        act = Q_NULL;
    }

    if (add)
    {
        QRect rcButton(rcControl.right() - rcControl.height(), rcControl.top(), rcControl.right()-(rcControl.right() - rcControl.height()), rcControl.bottom());
        rcButton.adjust(0,0,-1,0);
        but->setGeometry(rcButton);
        rcControl.setRight(rcControl.right() - rcButton.width());
    }
    return act;
}

ContextHeader* RibbonBarPrivate::hitTestContextHeaders(const QPoint& point) const
{
    for ( int i = 0, count = listContextHeaders_.count(); i < count; i++)
    {
        ContextHeader* header =  listContextHeaders_.at(i);
        if (header->rcRect.contains(point))
             return header;
    }
    return Q_NULL;
}

static QString findAccel(const QString& text)
{
    QString retAccel;
    bool found = false;
    int p = 0;
    while (p >= 0) 
    {
        p = text.indexOf(QLatin1Char('&'), p) + 1;
        if (p <= 0 || p >= (int)text.length())
            break;
        if (text.at(p) != QLatin1Char('&')) 
        {
            QChar c = text.at(p);
            if (c.isPrint()) 
            {
                if (!found) 
                {
                    c = c.toUpper();
                    retAccel = c.unicode();
                    found = true;
                }
            }
        }
        p++;
    }
    return retAccel;
}

void RibbonBarPrivate::showKeyTips(QWidget* w)
{
    hideKeyTips();
    countKey_ = 0;

    if (qobject_cast<RibbonBar*>(w))
        createKeyTips();
    else if (qobject_cast<RibbonPage*>(w))
        createGroupKeyTips();
    else if (qobject_cast<QMenu*>(w))
        createPopupKeyTips();

    int count = keyTips_.count();
    if (count == 0)
        return;

    for (int i = count - 1; i >= 0; i--)
    {
        RibbonKeyTip* keyTip = keyTips_.at(i);
        keyTip->setExplicit(!keyTip->getStringTip().isEmpty());

        if (keyTip->isExplicit())
            continue;

        QString strCaption = keyTip->getCaption();
        if (strCaption.isEmpty())
        {
            keyTip->close();
            keyTip->deleteLater();
            keyTips_.removeAt(i);
            continue;
        }

        strCaption = strCaption.toUpper();
        keyTip->setCaption(strCaption);

        if (keyTip->getPrefix().isEmpty())
        {
            QString key = ::findAccel(strCaption);
            if (key.isEmpty())
            {
                if (!strCaption.isEmpty())
                    keyTip->setStringTip(QChar(strCaption[0]));
            }
            else
                keyTip->setStringTip(key);
        }
        else
        {
            keyTip->setStringTip(keyTip->getPrefix());
        }
    }

    count = keyTips_.count();
    for (int i = 0; i < count; i++)
    {
        RibbonKeyTip* keyTip = keyTips_.at(i);

        QString strTip = keyTip->getStringTip();

        QList<RibbonKeyTip*> list;
        list.append(keyTip);

        int j;
        QString strUsed = QObject::tr("& ");

        for ( j = i + 1; j < count; j++)
        {
            RibbonKeyTip* keyTipWidget = keyTips_.at(j);;
            if (keyTipWidget->getStringTip()[0] == strTip[0])
            {
                list.append(keyTipWidget);
                if (keyTipWidget->getStringTip().length() == 2)
                    strUsed += keyTipWidget->getStringTip()[1];
            }
        }

        if (list.count() < 2)
            continue;

        int k = 1;
        for (int j = 0; j < list.count(); j++)
        {
            RibbonKeyTip* keyTipWidget = list.at(j);
            if (keyTipWidget->getStringTip().length() > 1)
                continue;

            QChar chAccel = 0;
            QString strCaption = keyTipWidget->getCaption();
            for (int n = 0; n < strCaption.length() - 1; n++)
            {
                if ((strCaption[n] == ' ') && (strUsed.indexOf(strCaption[n + 1]) == -1))
                {
                    chAccel = strCaption[n + 1];
                    strUsed += chAccel;
                    break;
                }
            }

            if (chAccel.isNull())
            {
                int start = strCaption[0] == QLatin1Char('&') ? 1 : 0;
                if (strCaption[start] == keyTipWidget->getStringTip()[0])
                    start++;

                for (int n = start; n < strCaption.length(); n++)
                {
                    if (strUsed.indexOf(strCaption[n]) == -1)
                    {
                        chAccel = keyTipWidget->getCaption()[n];
                        strUsed += chAccel;
                        break;
                    }
                }
            }
            if (chAccel == 0)
            {
                QString str = QObject::tr("%1").arg(k);
                chAccel = str[0];
                k++;
            }
            keyTipWidget->setStringTip(keyTipWidget->getStringTip() + chAccel);
        }
    }

    calcPositionKeyTips();

    if (keyTips_.count() > 0)
        qApp->installEventFilter(this);

    QTN_P(RibbonBar);
    p.setFocus();
}

bool RibbonBarPrivate::hideKeyTips()
{
    bool res = false;
    if (keyTips_.count() > 0)
    {
        qApp->removeEventFilter(this);
        res = true;
    }
    destroyKeyTips();
    countKey_ = 0;
    return res;
}

void RibbonBarPrivate::createKeyTips()
{
    QTN_P(RibbonBar);
    levels_.push(&p);

    QList<QToolButton*> list = quickAccessBar_->findChildren<QToolButton*>();
    for (int i = 0, count = list.count(); i < count; i++)
    {
        QToolButton* button = list.at(i);
        if (!button->isVisible() || button->objectName() == QLatin1String("QtnRibbonQuickAccessButton"))
            continue;

        int k = 1;
        if (QAction* act = button->defaultAction())
        {
            QList<QAction*> listActs = quickAccessBar_->actions();
            int index = listActs.indexOf(act);
            if (index != -1)
                k = index;
        }

        QString strCaption;
        if (k < 10)
            strCaption = QObject::tr("%1").arg(k);
        else if (k < 19)
            strCaption = QObject::tr("%1").arg(k - 9);
        else 
            break;
        keyTips_.append(new RibbonKeyTip(&p, button, strCaption, button->mapToGlobal(QPoint(button->rect().center().x(), button->rect().bottom() - 11)), 
            RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipTop, button->isEnabled()));
    }

    if (tabBar_ && tabBar_->isVisible())
    {
        for (int i = 0, count = tabBar_->getTabCount(); i < count; i++)
        {
            RibbonTab* tab = tabBar_->getTab(i);

            QString strCaption = tab->textTab();

            if (!KeyTipsComplement_ && ::findAccel(strCaption).isEmpty())
                strCaption = QString();

            if (!strCaption.isEmpty() && tab->isEnabled() && tab->isVisible())
            {
                QPoint pt(tab->rect().center().x(), tab->rect().bottom() - 9);
                RibbonKeyTip* keyTip = new RibbonKeyTip(&p, tab, strCaption, tab->mapToGlobal(pt), 
                    RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipTop, true);
                keyTips_.append(keyTip);
            }
        }
    }

    if (controlSystemButton_)
    {
        QPoint pt(controlSystemButton_->rect().center().x(), controlSystemButton_->rect().center().y());
        QString strCaption = controlSystemButton_->text();
        keyTips_.append(new RibbonKeyTip(&p, controlSystemButton_, strCaption, 
            controlSystemButton_->mapToGlobal(pt), RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipVCenter, controlSystemButton_->isEnabled()));
    }
}

void RibbonBarPrivate::createGroupKeyTips()
{
    int index = tabBar_->currentIndex();
    if (!validIndex(index))
        return;

    RibbonPage* page = listPages_.at(index);
    levels_.push(page);
    for (int i = 0, count = page->groupCount(); i < count; i++)
    {
        RibbonGroup* group = page->getGroup(i);
        bool visible = group->isVisible();

        QList<QWidget*> list = group->findChildren<QWidget*>();
        for (int j = 0; j < list.count(); j++)
        {
            QWidget* widget = list.at(j);
            createWidgetKeyTips(group, widget, QString(), page->rect().translated(page->mapToGlobal(page->rect().topLeft())), visible);
        }
    }
}

static QString getTextAction(const RibbonGroup& group, QWidget& widget)
{
    QString strCaption;
    foreach (QAction* action, group.actions())
    {
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action))
        {
            if (&widget == widgetAction->defaultWidget())
            {
                strCaption = widgetAction->text();
                break;
            }
        }
    }
    return strCaption;
}

void RibbonBarPrivate::createWidgetKeyTips(RibbonGroup* group, QWidget* widget, const QString& prefix, const QRect& rcGroups, bool visible)
{
    QTN_P(RibbonBar);

    if (!widget /*|| Qt::NoFocus == widget->focusPolicy()*/)
        return;

    QString strCaption;
    if (QAbstractButton* button = qobject_cast<QAbstractButton*>(widget))
    {
        strCaption = button->text();
        if(strCaption.isEmpty())
        {
            if (QToolButton* toolButton = qobject_cast<QToolButton*>(button))
            {
                if (QAction* action = toolButton->defaultAction())
                    strCaption = action->text();
                if (strCaption.isEmpty())
                {
                    if (qobject_cast<RibbonGallery*>(toolButton->parentWidget()))
                    {
                        if (toolButton->property(qtn_PopupButtonGallery).toBool())
                            strCaption = ::getTextAction(*group, *toolButton->parentWidget());
                    }
                    else if (strCaption.isEmpty())
                        strCaption = group->title();
                }
            }
        }
    }
    else if (ExWidgetWrapper* wrapper = qobject_cast<ExWidgetWrapper*>(widget))
        strCaption = wrapper->labelText();

    bool textWidget = true;
    if (RibbonGallery* gallery = qobject_cast<RibbonGallery*>(widget))
        textWidget = !gallery->getPopupMenu();

    if (strCaption.isEmpty() && textWidget)
        strCaption = ::getTextAction(*group, *widget);

    if (!KeyTipsComplement_ && ::findAccel(strCaption).isEmpty())
        strCaption = QString();

    if (strCaption.isEmpty())
        return;

    QRect rc = widget->rect();
    rc.translate(widget->mapToGlobal(rc.topLeft()));

    if (rc.isEmpty())
        return;

    QPoint pt(rc.left() + 21, rc.bottom() - 4);
    uint align = RibbonKeyTip::AlignTipTop;

    QStyleOption opt;
    opt.init(&p);

    QRect rcGroup = group->rect();
    rcGroup.translate(group->mapToGlobal(rcGroup.topLeft()));
    const int heightCaptionGroup = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, 0);
    QRect rcCaption(QPoint(rcGroup.left(), rcGroup.bottom() - heightCaptionGroup + 2), QPoint(rcGroup.right(), rcGroup.bottom()));
    rcGroup.setBottom(rcCaption.top());

    if (rc.intersects(rcCaption))
    {
        pt = QPoint(rc.center().x(), rcGroups.bottom() - 6);
        align = RibbonKeyTip::AlignTipTop | RibbonKeyTip::AlignTipCenter;
    }
    else if ((Qt::ToolButtonStyle)widget->property("toolButtonStyle").toInt() == Qt::ToolButtonTextUnderIcon && rc.height() > rcGroup.height() / 2)
    {
        pt = QPoint(rc.center().x(), rcGroup.bottom() - 2);
        align = RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipVCenter;
    }
    else if (qobject_cast<RibbonGallery*>(widget))
    {
        pt = QPoint(rc.right(), rcGroup.bottom() - 2);
        align = RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipVCenter;
    }
    else if (rc.center().y() < rcGroup.top() + rcGroup.height() * 1 / 3)
    {
        ///
        pt = QPoint(rc.left() + 11, rcGroup.top() + 2);
        align = RibbonKeyTip::AlignTipLeft | RibbonKeyTip::AlignTipVCenter;
    }
    else if (rc.center().y() > rcGroup.top() + rcGroup.height() * 2 / 3)
    {
        pt = QPoint(rc.left() + 11, rcGroup.bottom() - 2);
        align = RibbonKeyTip::AlignTipLeft | RibbonKeyTip::AlignTipVCenter;
    }
    else
    {
        pt = QPoint(rc.left() + 11, rcGroup.center().y() - 1);
        align = RibbonKeyTip::AlignTipLeft | RibbonKeyTip::AlignTipVCenter;
    }
    if (qobject_cast<QCheckBox*>(widget) || qobject_cast<QRadioButton*>(widget))
    {
        pt.setX(rc.left() + 6);
    }
/*
    if (qobject_cast<QComboBox*>(widget) || qobject_cast<QLineEdit*>(widget))
    {
        pt.setX(rc.center().x());
        align |= RibbonKeyTip::AlignTipCenter;
    }
*/
    RibbonKeyTip* keyTip = new RibbonKeyTip(&p, widget, strCaption, pt, align, widget->isEnabled());

    if (!prefix.isEmpty())
        keyTip->setPrefix(prefix);

    QRect rcIntersect; 
    rcIntersect = rcGroups.intersect(rc);
    keyTip->setVisibleTip(visible && (rcIntersect == rc) && (widget->isVisible()));
    keyTips_.append(keyTip);
}

bool RibbonBarPrivate::createPopupKeyTips()
{
    QTN_P(RibbonBar);
    if (QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget()))
    {
        levels_.push(menu);

        QList<QAction*> list = menu->actions();
        for (int i = 0; i < list.count(); i++)
        {
            QAction* action = list.at(i);
            QString strCaption = action->text();

            if (!KeyTipsComplement_ && ::findAccel(strCaption).isEmpty())
                strCaption = QString();

            if (!action->isVisible() || strCaption.isEmpty())
                continue;

            QRect rect = menu->actionGeometry(action);
            QPoint pt(rect.left() + 12, rect.bottom() - 11);
            RibbonKeyTip* keyTip = new RibbonKeyTip(&p, menu, strCaption, menu->mapToGlobal(pt), RibbonKeyTip::AlignTipTop, action->isEnabled(), action);

            keyTips_.append(keyTip);
        }

        QList<QWidget*> listWidget = menu->findChildren<QWidget*>();
        for (int i = 0; i < listWidget.count(); i++)
        {
            QWidget* widget = listWidget.at(i);

            QString strCaption;
            if (QToolButton* toolButton = qobject_cast<QToolButton*>(widget))
            {
                if (QAction* action = toolButton->defaultAction())
                    strCaption = action->text();
            }

            if (!KeyTipsComplement_ && ::findAccel(strCaption).isEmpty())
                strCaption = QString();

            if (!widget->isVisible() || strCaption.isEmpty())
                continue;

            QPoint pt(widget->rect().left() + 12, widget->rect().bottom() - 11);
            RibbonKeyTip* keyTip = new RibbonKeyTip(&p, widget, strCaption, widget->mapToGlobal(pt), 
                RibbonKeyTip::AlignTipTop, widget->isEnabled());
            keyTips_.append(keyTip);
        }
        return true;
    }
    return false;
}

void RibbonBarPrivate::destroyKeyTips()
{
    for (int i = 0; i < keyTips_.count(); i++)
    {
        RibbonKeyTip* keyTip = keyTips_.at(i);
        keyTip->close();
        keyTip->deleteLater();
    }
    keyTips_.clear();
}

void RibbonBarPrivate::calcPositionKeyTips()
{
    for (int i = 0, count = keyTips_.count(); i < count; i++)
    {
        RibbonKeyTip* keyTip = keyTips_.at(i);
        if (!keyTip->isVisibleTip())
            continue;

        QSize sz(0, 0);
        QFontMetrics fontMetrics(keyTip->font());
        QRect rect = fontMetrics.boundingRect(keyTip->getStringTip());
        sz = QSize(qMax(rect.width() + 6, 16), qMax(rect.height() + 2, 15));

        QPoint pt = keyTip->posTip();
        uint flagAlign = keyTip->getAlign();

        QRect rc(QPoint(pt.x() - ((flagAlign & RibbonKeyTip::AlignTipCenter) ?  sz.width() / 2 : (flagAlign & RibbonKeyTip::AlignTipRight) ? sz.width() : 0),
            pt.y() - (flagAlign & RibbonKeyTip::AlignTipVCenter ?  sz.height() / 2 : flagAlign & RibbonKeyTip::AlignTipBottom ? sz.height() : 0)), sz);

        keyTip->setGeometry(rc);
        keyTip->show();
        keyTip->repaint();
    }
}

int RibbonBarPrivate::getMaxHeightPages() const
{
    QTN_P(const RibbonBar);
    QStyleOption opt;
    int maxHeightPages =  p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, &opt, 0);
    for (int i = 0, count = listPages_.count(); count > i; i++)
    {
        RibbonPage* page = listPages_[i];
        for (int g = 0, countG = page->groupCount(); countG > g; g++)
        {
            if (RibbonGroup* group = page->getGroup(g))
                maxHeightPages = qMax(maxHeightPages, group->sizeHint().height());
        }
    }
    return maxHeightPages;
}

bool RibbonBarPrivate::pressTipCharEvent(const QKeyEvent* key)
{
    QTN_P(RibbonBar);
    QString str = key->text().toUpper();
    if (str.isEmpty())
        return false;
    for (int i = 0; i < keyTips_.count(); i++)
    {
        RibbonKeyTip* keyTip = keyTips_.at(i);
        int length = keyTip->getStringTip().length();
        if (length > countKey_)
        {
            if (keyTip->getStringTip()[countKey_] == str[0])
            {
                if (length - 1 > countKey_)
                {
                    countKey_++;

                    QString str = keyTip->getStringTip().left(countKey_);
                    for (int j = keyTips_.count() - 1; j >= 0; j--)
                    {
                        keyTip = keyTips_.at(j);
                        if (keyTip->getStringTip().left(countKey_) != str)
                        {
                            keyTips_.removeAt(j);
                            keyTip->hide();
                            keyTip->deleteLater();
                        }
                    }
                }
                else
                {
                    if (QAbstractButton* absButton = qobject_cast<QAbstractButton*>(keyTip->getOwner()))
                    {
                        if (keyTip->isEnabledTip() && keyTip->isVisibleTip())
                        {
                            hideKeyTips();
                            bool clearLevel = true;
                            if (QToolButton* button = qobject_cast<QToolButton*>(absButton))
                            {
                                if (button->popupMode() == QToolButton::InstantPopup)
                                    clearLevel = false;
                            }
                            if (clearLevel)
                                levels_.clear();
                            absButton->animateClick(0);
                        }
                        return false;
                    }
                    else if (qobject_cast<QMenu*>(keyTip->getOwner()) && keyTip->isEnabledTip() && keyTip->isVisibleTip())
                    {
                        hideKeyTips();
                        levels_.clear();
                        if (QAction* action =  keyTip->getAction())
                        {
                            action->trigger();
                            keyTip->getOwner()->close();
                            return false;
                        }
                    } 
                    else if (qobject_cast<RibbonTab*>(keyTip->getOwner()))
                    {
                        KeyTipEvent kte(keyTip);
                        QApplication::sendEvent(&p, &kte);
                        return false;
                    }
                    else if (keyTip->getOwner() && keyTip->isEnabledTip() && keyTip->isVisibleTip())
                    {
                        hideKeyTips();
                        levels_.clear();
                        keyTip->getOwner()->setFocus();
                    }
                }
                return false;
            }
        }
    }
    return false;
}

bool RibbonBarPrivate::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type()) 
    {
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::NonClientAreaMouseButtonPress :
        case QEvent::NonClientAreaMouseButtonRelease :
        case QEvent::NonClientAreaMouseButtonDblClick :
            {
                hideKeyTips();
                levels_.clear();
            }
            break;
        case QEvent::KeyPress :
            if (QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event))
            {
                if (keyEvent->key() != Qt::Key_Escape)
                {
                    if ((qobject_cast<QMainWindow*>(obj) || (minimized_ && qobject_cast<RibbonPage*>(obj))))
                        pressTipCharEvent(keyEvent);
                    else if (qobject_cast<QMenu*>(obj))
                        return !pressTipCharEvent(keyEvent);
                }
                else
                {
                    if (levels_.size() > 1 && levels_.pop())
                        showKeyTips(levels_.pop());
                    else if (levels_.size() > 0 && levels_.pop())
                        hideKeyTips();
                }
            }
            break;
        case QEvent::Close :
            if (minimized_)
            {
                if (levels_.size() > 1 && levels_.pop())
                    showKeyTips(levels_.pop());
            }
            break;

        default:
            break;
    }
    return false;
}

void RibbonBarPrivate::processClickedSubControl()
{
    QTN_P(RibbonBar);
    if (QAction* action = qobject_cast<QAction *>(sender()))
    {
        QWidget* widget = p.parentWidget();
        if (widget == Q_NULL )
            return;

        if (actCloseButton_ == action)
            widget->close();
        else if (actNormalButton_ == action)
            widget->showNormal();
        else if (actMaxButton_ == action)
            widget->showMaximized();
        else if (actMinButton_ == action)
            widget->showMinimized();
    }
}



/* RibbonBar */
RibbonBar::RibbonBar(QWidget* parent)
    : QMenuBar(parent)
{
    QTN_INIT_PRIVATE(RibbonBar);
    QTN_D(RibbonBar);
    d.init();
}

RibbonBar::~RibbonBar()
{
    QTN_FINI_PRIVATE();
}

bool RibbonBar::isVisible() const
{
    QTN_D(const RibbonBar);
    return QMenuBar::isVisible() && d.ribbonBarVisible_;
}

RibbonPage* RibbonBar::addPage(const QString& text)
{
    return insertPage(-1, text);
}

void RibbonBar::addPage(RibbonPage* page)
{
    insertPage(-1, page);
}

RibbonPage* RibbonBar::insertPage(int index, const QString& text)
{
    QTN_D(RibbonBar);
    int indTab = d.tabBar_->insertTab(index, text);

    RibbonPage* page = d.insertPage(indTab, index);
    Q_ASSERT(page != Q_NULL);
    d.updateMinimizedModePage(page);
    return page;
}

void RibbonBar::insertPage(int index, RibbonPage* page)
{
    Q_ASSERT(page != Q_NULL);

    QTN_D(RibbonBar);
    page->setParent(this);
    int indTab = d.tabBar_->insertTab(index, page->title());
    page->hide();
    d.insertPage(indTab, index, page);
    d.updateMinimizedModePage(page);
}

void RibbonBar::movePage(RibbonPage* page, int newIndex)
{
    QTN_D(RibbonBar);
    movePage(d.getIndexPage(page), newIndex);
}

void RibbonBar::movePage(int index, int newIndex)
{
    QTN_D(RibbonBar);
    if (d.validIndex(index) && d.validIndex(newIndex))
    {
        d.tabBar_->moveTab(index, newIndex);
        d.listPages_.move(index, newIndex);
        d.layoutRibbon();
    }
}

void RibbonBar::removePage(RibbonPage* page)
{
    QTN_D(RibbonBar);
    int index = d.getIndexPage(page);
    if (index != -1)
        removePage(index);
}

void RibbonBar::removePage(int index)
{
    QTN_D(RibbonBar);
    d.removePage(index);
    d.tabBar_->removeTab(index);
    d.layoutRibbon();
}

void RibbonBar::detachPage(RibbonPage* page)
{
    QTN_D(RibbonBar);
    int index = d.getIndexPage(page);
    if (index != -1)
        detachPage(index);
}

void RibbonBar::detachPage(int index)
{
    QTN_D(RibbonBar);
    d.removePage(index, false);
    d.tabBar_->removeTab(index);
    d.layoutRibbon();
}

void RibbonBar::clearPages()
{
    for (int i = getPageCount() - 1; i >= 0; i--)
        removePage(i);
}

bool RibbonBar::isKeyTipsComplement() const
{
    QTN_D(const RibbonBar);
    return d.KeyTipsComplement_;
}

void RibbonBar::setKeyTipsComplement(bool complement)
{
    QTN_D(RibbonBar);
    d.KeyTipsComplement_ = complement;
}

RibbonQuickAccessBar* RibbonBar::getQuickAccessBar() const
{
    QTN_D(const RibbonBar);
    return d.quickAccessBar_;
}

void RibbonBar::showQuickAccess(bool show)
{
    QTN_D(RibbonBar);
    d.quickAccessBar_->setVisible(show);
}

bool RibbonBar::isQuickAccessVisible() const
{
    QTN_D(const RibbonBar);
    return d.quickAccessBar_ && d.quickAccessBar_->isVisible();
}

void RibbonBar::setMinimizedFlag(bool flag)
{
    QTN_D(RibbonBar);
    if (d.minimized_ != flag)
    {
        d.minimized_ = flag;

        if (!d.minimized_ && (d.tabBar_ && d.tabBar_->getTabCount() > 0 && d.tabBar_->currentIndex() == -1))
            setCurrentPage(0);

        if (d.minimized_)
        {
            for (int i = 0, count = d.listPages_.size(); count > i; i++)
                d.updateMinimizedModePage(d.listPages_.at(i));
        }
        else
        {
            for (int i = 0, count = d.listPages_.size(); count > i; i++)
                d.updateMinimizedModePage(d.listPages_.at(i));
            currentChanged(currentIndexPage());
        }

        d.tabBar_->refresh();

        d.layoutRibbon();
        adjustSize();

        emit minimizationChanged(d.minimized_);
    }
}

void RibbonBar::minimize()
{
    setMinimizedFlag(true);
}

bool RibbonBar::isMinimized() const
{
    QTN_D(const RibbonBar);
    return d.minimized_;
}

void RibbonBar::setMinimized(bool flag)
{
    setMinimizedFlag(flag);
}

void RibbonBar::maximize()
{
    setMinimizedFlag(false);
}

bool RibbonBar::isMaximized() const
{
    return !isMinimized();
}

void RibbonBar::setMaximized(bool flag)
{
    setMinimizedFlag(!flag);
}

void RibbonBar::setMinimizationEnabled(bool enabled)
{
    QTN_D(RibbonBar);
    d.minimizationEnabled_ = enabled;
}

bool RibbonBar::isMinimizationEnabled() const
{
    QTN_D(const RibbonBar);
    return d.minimizationEnabled_;
}

void RibbonBar::setCurrentPage(int index)
{
    QTN_D(RibbonBar);
    d.tabBar_->setCurrentIndex(index);
}

int RibbonBar::currentIndexPage() const
{
    QTN_D(const RibbonBar);
    return d.tabBar_->currentIndex();
}

RibbonPage* RibbonBar::getPage(int index) const
{
    QTN_D(const RibbonBar);
    if (d.validIndex(index))
        return d.listPages_[index];
    return Q_NULL;
}

int RibbonBar::getPageCount() const
{
    QTN_D(const RibbonBar);
    return d.listPages_.count();
}

QMenu* RibbonBar::addMenu(const QString& text)
{
    QTN_D(RibbonBar);
    return d.tabBar_->addMenu(text);
}

QAction* RibbonBar::addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, QMenu* menu)
{
    QTN_D(RibbonBar);
    return d.tabBar_->addAction(icon, text, style, menu);
}

QAction* RibbonBar::addSystemButton(const QString& text)
{
    return addSystemButton(QIcon(), text);
}

QAction* RibbonBar::addSystemButton(const QIcon& icon, const QString& text)
{
    QTN_D(RibbonBar);
    QAction* actionSystemPopupBar = d.controlSystemButton_ ? 
        d.controlSystemButton_->defaultAction() : d.createSystemButton(icon, text);

    if (QMenu* menu = actionSystemPopupBar->menu())
        delete menu;

    actionSystemPopupBar->setMenu(new RibbonSystemPopupBar(this));

    return actionSystemPopupBar;
}

RibbonSystemButton* RibbonBar::getSystemButton() const
{
    QTN_D(const RibbonBar);
    return d.controlSystemButton_;
}

bool RibbonBar::isBackstageVisible() const
{
    QTN_D(const RibbonBar);
    if (d.controlSystemButton_)
    {
        if (RibbonBackstageView* backstage = qobject_cast<RibbonBackstageView*>(d.controlSystemButton_->backstage()))
            return backstage->isVisible();
    }
    return false;
}

void RibbonBar::setFrameThemeEnabled(bool enable)
{
#ifdef Q_OS_WIN
    QTN_D(RibbonBar);
    if (!enable)
    {
        if (d.frameHelper_)
        {
            delete d.frameHelper_;
            d.frameHelper_ = Q_NULL;
        }

        d.addTitleButton(QStyle::SC_TitleBarCloseButton, false, d.rcHeader_);
        d.addTitleButton(QStyle::SC_TitleBarNormalButton, false, d.rcHeader_);
        d.addTitleButton(QStyle::SC_TitleBarMaxButton, false, d.rcHeader_);
        d.addTitleButton(QStyle::SC_TitleBarMinButton, false, d.rcHeader_);
        adjustSize();
    }
    else if (!d.frameHelper_)
    {
        d.frameHelper_ = new OfficeFrameHelperWin(this);
        d.frameHelper_->enableOfficeFrame(this->parentWidget());
        d.frameHelper_->setContextHeader(&d.listContextHeaders_);
    }
#else // Q_OS_WIN
    Q_UNUSED(enable)
#endif // Q_OS_WIN
}

bool RibbonBar::isFrameThemeEnabled() const
{
    QTN_D(const RibbonBar);
    return d.frameHelper_;
}

int RibbonBar::tabBarHeight() const
{
    QTN_D(const RibbonBar);
    return (d.tabBar_ && d.tabBar_->isVisible()) ? d.rcTabBar_.height() : 2;
}

bool RibbonBar::isTitleBarVisible() const
{
    QTN_D(const RibbonBar);
    return isFrameThemeEnabled() || d.showTitleAlways_;
}

void RibbonBar::setTitleBarVisible(bool show)
{
    QTN_D(RibbonBar);
    d.showTitleAlways_ = show;
    d.layoutRibbon();
}

int RibbonBar::titleBarHeight() const
{
    QTN_D(const RibbonBar);
    if (!isFrameThemeEnabled() && !d.showTitleAlways_)
        return 0;

    int height = style()->pixelMetric(QStyle::PM_TitleBarHeight, 0);

    if (!isQuickAccessVisible() || !d.quickAccessOnTop_)
        return height + (parentWidget()->windowState() & Qt::WindowMaximized ? topBorder()/2 : topBorder() + 1);

    int quickAccessHeight = getQuickAccessBar()->height();
    quickAccessHeight = qMax(quickAccessHeight, d.tabBar_->height() - 2);

    if (height >= quickAccessHeight)
        return height;

    return quickAccessHeight;
}

int RibbonBar::topBorder() const
{
    QTN_D(const RibbonBar);
    return d.frameHelper_ && d.frameHelper_->isDwmEnabled() ? d.frameHelper_->getFrameBorder() : 0;
}

void RibbonBar::currentChanged(int index)
{
    QTN_D(RibbonBar);
    d.currentChanged(index);
}

bool RibbonBar::event(QEvent* event)
{
    QTN_D(RibbonBar);

    bool result = QMenuBar::event(event);

    switch(event->type())
    {
        case QEvent::LayoutRequest :
                d.layoutRibbon();
            break;
        case QEvent::WindowTitleChange :
            {
                d.windowTitle_ = parentWidget() ? parentWidget()->windowTitle() : QString();
                QApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
            }
            break;
        case QEvent::WindowIconChange:
            if (d.frameHelper_)
                d.frameHelper_->resetIcon();
            break;
        default:
            break;
    }

    if (event->type() == KeyTipEvent::eventNumber())
    {
        KeyTipEvent* keyTipEvent = static_cast<KeyTipEvent*>(event);
        if (RibbonTab* tab = qobject_cast<RibbonTab*>(keyTipEvent->getKeyTip()->getOwner()))
        {
            int index = d.tabBar_->getIndex(tab);
            if (index != -1)
            {
                setCurrentPage(index);
                d.showKeyTips(getPage(index));
            }
        }
    }
    else if (event->type() == ShowKeyTipEvent::eventNumber())
    {
        ShowKeyTipEvent* showKeyTipEvent = static_cast<ShowKeyTipEvent*>(event);
        if (QWidget* widget = showKeyTipEvent->getWidget())
        {
            if (d.levels_.indexOf(widget) == -1)
                d.showKeyTips(widget);
        }
    }
    else if (event->type() == HideKeyTipEvent::eventNumber())
    {
        d.hideKeyTips();
        d.levels_.clear();
    }
    return result;
}

bool RibbonBar::eventFilter(QObject* object, QEvent* event)
{
    QTN_D(RibbonBar);
    const QEvent::Type type = event->type();

    if (type == QEvent::Resize)
    {
        if (object == parentWidget() && !isVisible())
        {
            d.layoutRibbon();
            return true;
        }
    } 

    return QMenuBar::eventFilter(object, event);
}

void RibbonBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QTN_D(RibbonBar);
    QWidget* pLeftCorner = cornerWidget(Qt::TopLeftCorner);
    if(pLeftCorner && pLeftCorner->isVisible())
        pLeftCorner->setVisible(false);

    // draw ribbon
    QPainter p(this);

    StyleOptionRibbon opt;
    opt.init(this);
    opt.frameHelper = d.frameHelper_;
    opt.rectTabBar = d.rcTabBar_;
    opt.minimized = d.minimized_;
    opt.titleBarHeight = titleBarHeight();

//    if (isFrameThemeEnabled() && d.rcTitle_.isValid() && d.frameHelper_->isDwmEnabled())
//        opt.rect.setTop(d.rcTitle_.height());

    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonBar, &opt, &p, this);

    if ((isFrameThemeEnabled() || d.showTitleAlways_) &&  d.rcTitle_.isValid())
    {
        StyleOptionTitleBar titleBarOptions;
        d.titleBarOptions(titleBarOptions);
        titleBarOptions.frameHelper = d.frameHelper_;
        titleBarOptions.rectTabBar = d.rcTabBar_;
        titleBarOptions.rcRibbonClient = rect();
        titleBarOptions.listContextHeaders = &d.listContextHeaders_;

        QFont saveFont = p.font();
        const QFont font = QApplication::font("QWorkspaceTitleBar");
        p.setFont(font);

        style()->drawComplexControl(QStyle::CC_TitleBar, &titleBarOptions, &p, this);

        p.setFont(saveFont);
    }

    opt.rect = d.rcPageArea_;
    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this);
}

void RibbonBar::changeEvent(QEvent* event)
{
    QTN_D(RibbonBar);
    switch (event->type()) 
    {
        case QEvent::FontChange: 
            {
                for (int i = 0, count = d.listPages_.count(); count > i; i++)
                {
                    RibbonPage* page = d.listPages_[i];
                    page->setFont(font());
                    QEvent ev(QEvent::LayoutRequest);
                    QApplication::sendEvent(page, &ev);
                }

                QList<QMenu*> widgets = qFindChildren<QMenu*>(this);
                foreach (QMenu* wd, widgets)
                    wd->setFont(font());

                QEvent ev(QEvent::LayoutRequest);
                QApplication::sendEvent(this, &ev);
                updateGeometry();
            }
            break;
        default:
            break;
    };
    return QMenuBar::changeEvent(event);
}

void RibbonBar::resizeEvent(QResizeEvent* ev)
{
    QMenuBar::resizeEvent(ev);
    QTN_D(RibbonBar);
    d.layoutRibbon();
}

void RibbonBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    QMenuBar::mouseDoubleClickEvent(event);

    if (event->button() != Qt::LeftButton)
        return;

    QTN_D(RibbonBar);
    if (isFrameThemeEnabled() && isTitleBarVisible())
    {
        if (d.hitTestContextHeaders(event->pos()))
        {
            QWidget* widget = parentWidget();
            if (widget == Q_NULL )
                return;

            StyleOptionTitleBar titleBarOpt;
            d.titleBarOptions(titleBarOpt);

            bool buttonMax =  titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint
                && !(titleBarOpt.titleBarState & Qt::WindowMaximized); 

            bool buttonNormal = (((titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint)
                && (titleBarOpt.titleBarState & Qt::WindowMinimized))
                || ((titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint)
                && (titleBarOpt.titleBarState & Qt::WindowMaximized)));

            Qt::WindowStates titleBarState = widget->windowState();

            if (buttonMax)
                widget->showMaximized();
            else if (buttonNormal)
                widget->showNormal();

            QSize s = widget->size();
            if (s.isValid())
                widget->resize(s);

            return;
        }
    }

    if (d.minimizationEnabled_ && d.tabBar_->getTab(d.tabBar_->currentIndex()) == childAt(event->pos()))
    {
        if (isMinimized())
            setMinimizedFlag(false);
        else
            setMinimizedFlag(true);
    }
}

void RibbonBar::mousePressEvent(QMouseEvent* event)
{
    QMenuBar::mousePressEvent(event);

    QTN_D(RibbonBar);
    if (event->button() == Qt::LeftButton)
    {
        if (ContextHeader* header = d.hitTestContextHeaders(event->pos()))
        {
            for (int i = 0, count = d.tabBar_->getTabCount(); count > i; i++)
            {
                if (RibbonTab* tab = d.tabBar_->getTab(i))
                {
                    if (tab == header->firstTab)
                    {
                        setCurrentPage(i);
                        break;
                    }
                }
            }
        }
    }
}

int RibbonBar::heightForWidth(int) const
{
    ensurePolished();
    QTN_D(const RibbonBar);

    const int tabsHeight = d.rcTabBar_.height();
    const int pageAreaHeight = d.rcPageArea_.height();
    const int ribbonTopBorder = d.frameHelper_ && d.frameHelper_->isDwmEnabled() ? d.frameHelper_->getFrameBorder() : 0;
    const int heightTitle = d.frameHelper_ ? titleBarHeight() : d.rcTitle_.height();

    int height = tabsHeight + pageAreaHeight + heightTitle + ribbonTopBorder;
    if (d.minimized_)
        height += 3;

    if (d.ribbonBarVisible_)
        return height;

    return 0;
}

QSize RibbonBar::sizeHint() const
{
    return QSize(rect().width(), heightForWidth(0)).expandedTo(QApplication::globalStrut());
}

#ifdef Q_OS_WIN
bool RibbonBar::winEvent(MSG* message, long* result)
{
    QTN_D(RibbonBar);
    if (d.frameHelper_ && d.frameHelper_->winEvent(message, result))
        return true;
    return QMenuBar::winEvent(message, result);
}
#endif // Q_OS_WIN
