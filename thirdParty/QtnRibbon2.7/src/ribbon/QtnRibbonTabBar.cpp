/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonTabBar for Qt)
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
#include <QStyleOption>
#include <qevent.h>

#include "QtnRibbonBar.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonButton.h"
#include "QtnRibbonTabBar.h"
#include "QtnRibbonPage.h"
#include "QtnRibbonBackstageView.h"

#include "QtnRibbonQuickAccessBar.h"
#include "QtnRibbonSystemPopupBar.h"

using namespace Qtitan;

namespace Qtitan
{
/*!
\class Qtitan::ContextHeader
\internal
*/
    ContextHeader::ContextHeader(RibbonTab* tab)
    {
        Q_ASSERT(tab);
        if (!tab)
            return;

        lastTab = firstTab = tab;
        color = firstTab->getContextColor();
        strTitle = firstTab->contextTextTab();
        rcRect = QRect();
        firstTab->setContextHeader(this);
    }
    ContextHeader::~ContextHeader()
    {
        firstTab->setContextHeader(Q_NULL);
    }
};

namespace Qtitan
{
    /* RibbonTabPrivate */
    class RibbonTabPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonTab)

    public:
        explicit RibbonTabPrivate();

    public:
        void init();
        QSize sizeForWidth(int w) const;
        void updateLabel();
        QRect documentRect() const;
        QRectF layoutRect() const;

    public:
        mutable bool valid_hints_;
        mutable QSize sh_;
        mutable QSizePolicy sizePolicy_;
        mutable QSize msh_;

        bool select_;
        bool tracking_;
        bool minimizeDblClick_;
        int margin_;
        Qt::Alignment align_;
        short indent_;
        short frameWidth_;
        QString text_;
        QString contextText_;
        ContextColor contextColor_;
        ContextHeader* contextHeader_;

        int shortcutId_;

        mutable uint isTextLabel_     : 1;
        mutable uint textLayoutDirty_ : 1;
        mutable uint textDirty_       : 1;
        mutable uint tabMouseOver_    : 1;
    };
};//namespace Qtitan

RibbonTabPrivate::RibbonTabPrivate()
    : shortcutId_(0)
{
}

void RibbonTabPrivate::init()
{
    QTN_P(RibbonTab);
    contextColor_ = ContextColorNone;
    contextHeader_ = Q_NULL;
    valid_hints_   = false;
    margin_        = 0;
    isTextLabel_   = false;
    textDirty_     = false;
    tabMouseOver_  = false;
    select_        = false;
    tracking_      = false;
    minimizeDblClick_ = false;
    shortcutId_ = -1;
    align_ = Qt::AlignLeft | Qt::AlignVCenter /*| Qt::TextExpandTabs*/;
    indent_ = -1;
    p.setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::Label));
}

QSize RibbonTabPrivate::sizeForWidth(int w) const
{
    QTN_P(const RibbonTab);
    if(p.minimumWidth() > 0)
        w = qMax( w, p.minimumWidth() );

    QSize contentsMargin(2,0);

    QRect br;

    int hextra = 2 * margin_;
    int vextra = hextra;
    QFontMetrics fm = p.fontMetrics();

    if (!text_.isEmpty()) 
    {
        int align_ = QStyle::visualAlignment(p.layoutDirection(), QFlag(this->align_));
        // Add indentation
        int m = indent_;

        if (m < 0 && frameWidth_) // no indent_, but we do have a frame
            m = fm.width(QLatin1Char('x')) - margin_*2;
        if (m > 0) 
        {
            if ((align_ & Qt::AlignLeft) || (align_ & Qt::AlignRight))
                hextra += m;
            if ((align_ & Qt::AlignTop) || (align_ & Qt::AlignBottom))
                vextra += m;
        }

        // Turn off center alignment in order to avoid rounding errors for centering,
        // since centering involves a division by 2. At the end, all we want is the size.
        int flags = align_ & ~(Qt::AlignVCenter | Qt::AlignHCenter);

        bool tryWidth = (w < 0) && (align_ & Qt::TextWordWrap);
        if (tryWidth)
            w = fm.averageCharWidth() * 80;
        else if (w < 0)
            w = 2000;
        w -= (hextra + contentsMargin.width());
        br = fm.boundingRect(0, 0, w ,2000, flags, text_);
        if (tryWidth && br.height() < 4*fm.lineSpacing() && br.width() > w/2)
            br = fm.boundingRect(0, 0, w/2, 2000, flags, text_);
        if (tryWidth && br.height() < 2*fm.lineSpacing() && br.width() > w/4)
            br = fm.boundingRect(0, 0, w/4, 2000, flags, text_);
    } else
        br = QRect(QPoint(0, 0), QSize(fm.averageCharWidth(), fm.lineSpacing()));

    const QSize contentsSize(br.width() + hextra+12, br.height() + vextra);
    return (contentsSize + contentsMargin).expandedTo(p.minimumSize());
}

void RibbonTabPrivate::updateLabel()
{
    QTN_P(RibbonTab);
    valid_hints_ = false;

    if (isTextLabel_)
    {
        QSizePolicy policy = p.sizePolicy();
        const bool wrap = align_ & Qt::TextWordWrap;
        policy.setHeightForWidth(wrap);
        // should be replaced by WA_WState_OwnSizePolicy idiom
        if (policy != p.sizePolicy())  
            p.setSizePolicy(policy);
        textLayoutDirty_ = true;
    }
    p.updateGeometry();
    p.update(p.contentsRect());
}

/*
 * Returns the rect that is available for us to draw the document
 */
QRect RibbonTabPrivate::documentRect() const
{
    QTN_P(const RibbonTab);
    Q_ASSERT_X(isTextLabel_, "documentRect", "document rect called for tab that is not a text_ tab!");
    QRect cr = p.contentsRect();
    cr.setLeft(cr.left());
    cr.adjust(margin_, margin_, -margin_, -margin_);
    const int align_ = QStyle::visualAlignment(p.layoutDirection(), QFlag(this->align_));
    int m = indent_;
    if (m < 0 && frameWidth_ ) // no indent_, but we do have a frame
        m = p.fontMetrics().width(QLatin1Char('x')) / 2 - margin_;
    if (m > 0) 
    {
        if (align_ & Qt::AlignLeft)
            cr.setLeft(cr.left() + m);
        if (align_ & Qt::AlignRight)
            cr.setRight(cr.right() - m);
        if (align_ & Qt::AlignTop)
            cr.setTop(cr.top() + m);
        if (align_ & Qt::AlignBottom)
            cr.setBottom(cr.bottom() - m);
    }
    return cr;
}

QRectF RibbonTabPrivate::layoutRect() const
{
    return documentRect();
}


/* RibbonTab */
RibbonTab::RibbonTab(const QString& text, QWidget* parent)
    : QWidget(parent)
    , lastTab(-1)
{
    QTN_INIT_PRIVATE(RibbonTab);
    QTN_D(RibbonTab);

    d.init();
    setTextTab(text);
    setMargin(4);
    setAlignment(Qt::AlignCenter);
}

RibbonTab::~RibbonTab()
{
    QTN_D(RibbonTab);
    QWidget* pParent = parentWidget();
    pParent->releaseShortcut(d.shortcutId_);
    QTN_FINI_PRIVATE();
}

void RibbonTab::setSelect(bool select)
{
    QTN_D(RibbonTab);
    d.select_ = select;
    d.updateLabel();
}

bool RibbonTab::select() const
{
    QTN_D(const RibbonTab);
    return d.select_;
}

void RibbonTab::setIndent(int indent)
{
    QTN_D(RibbonTab);
    d.indent_ = indent;
    d.updateLabel();
}

int RibbonTab::indent() const
{
    QTN_D(const RibbonTab);
    return d.indent_;
}

int RibbonTab::margin() const
{
    QTN_D(const RibbonTab);
    return d.margin_;
}

void RibbonTab::setMargin(int margin)
{
    QTN_D(RibbonTab);
    if (d.margin_ == margin)
        return;
    d.margin_ = margin;
    d.updateLabel();
}

Qt::Alignment RibbonTab::alignment() const
{
    QTN_D(const RibbonTab);
    return QFlag(d.align_ & (Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask));
}

void RibbonTab::setAlignment(Qt::Alignment alignment)
{
    QTN_D(RibbonTab);
    if (alignment == (d.align_ & (Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask)))
        return;
    d.align_ = (d.align_ & ~(Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask)) | 
        (alignment & (Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask));

    d.updateLabel();
}

void RibbonTab::setTextTab(const QString& text)
{
    QTN_D(RibbonTab);
    if (d.text_ == text)
        return;
    d.text_ = text;
    d.isTextLabel_ = true;
    d.textDirty_ = true;
    d.updateLabel();

    QWidget* pParent = parentWidget();
    pParent->releaseShortcut(d.shortcutId_);
    d.shortcutId_ = pParent->grabShortcut(QKeySequence::mnemonic(text));
    pParent->setShortcutEnabled(d.shortcutId_, isEnabled());

    QApplication::postEvent(pParent, new QEvent(QEvent::LayoutRequest));
}

void RibbonTab::setContextTextTab(const QString& contextText)
{
    QTN_D(RibbonTab);
    d.contextText_ = contextText;
}

const QString& RibbonTab::contextTextTab() const
{
    QTN_D(const RibbonTab);
    return d.contextText_;
}

void RibbonTab::setContextTab(ContextColor color)
{
    QTN_D(RibbonTab);
    d.contextColor_ = color;
    if (d.contextHeader_)
        d.contextHeader_->color = color;
}

ContextColor RibbonTab::getContextColor() const
{
    QTN_D(const RibbonTab);
    return d.contextColor_;
}

const QString& RibbonTab::textTab() const
{
    QTN_D(const RibbonTab);
    return d.text_;
}

void RibbonTab::setContextHeader(ContextHeader* contextHeader)
{
    QTN_D(RibbonTab);
    d.contextHeader_ = contextHeader;
}

ContextHeader* RibbonTab::getContextHeader() const
{
    QTN_D(const RibbonTab);
    return d.contextHeader_;
}

void RibbonTab::setTrackingMode(bool tracking)
{
    QTN_D(RibbonTab);
    d.tracking_ = tracking;
    repaint();
}

bool RibbonTab::isTrackingMode() const
{
    QTN_D(const RibbonTab);
    return d.tracking_;
}

int RibbonTab::shortcut() const
{
    QTN_D(const RibbonTab);
    return d.shortcutId_;
}

bool RibbonTab::validRect() const
{
    QTN_D(const RibbonTab);
    int wid = 0;    
    QStyleOptionHeader opt;
    opt.init(this);
    int clientWidth = opt.rect.width();
    if (!d.text_.isEmpty())
    {
        QString text = d.text_;
        wid = opt.fontMetrics.width(text.remove(QChar('&')));
    }
    else
        wid = 10;

    return clientWidth >= wid;
}

bool RibbonTab::event(QEvent* event)
{
    return QWidget::event(event);
}

void RibbonTab::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QTN_D(RibbonTab);
    QPainter p(this);

    QStyleOptionHeader opt;
    opt.init(this);
    opt.text = p.fontMetrics().elidedText(d.text_, Qt::ElideRight, opt.rect.adjusted(2,0,-2,0).width());
    opt.textAlignment = d.align_;

    bool minimized = false;
    bool backstageVisible = false;
    if (RibbonBar* rb = qobject_cast<RibbonBar*>(parentWidget()->parentWidget()))
    {
        minimized = rb->isMinimized();
        backstageVisible = rb->isBackstageVisible();
    }

    if (!minimized && !backstageVisible)
    {
        if (d.select_)
            opt.state |= QStyle::State_Selected;
        else
            opt.state &= ~QStyle::State_Selected;
    }
    else
    {
        if (d.tracking_)
        {
            opt.state |= QStyle::State_Selected;
            opt.state |= QStyle::State_Sunken;
        }
        else
        {
            opt.state &= ~QStyle::State_Selected;
            opt.state &= ~QStyle::State_Sunken;
        }
    }

    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonTab, &opt, &p, this);
    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonTabShapeLabel, &opt, &p, this);
}

QSize RibbonTab::sizeHint() const
{
    QTN_D(const RibbonTab);
    if (!d.valid_hints_)
        (void)RibbonTab::minimumSizeHint();
    return d.sh_;
}

QSize RibbonTab::minimumSizeHint() const
{
    QTN_D(const RibbonTab);
    if (d.valid_hints_ && d.sizePolicy_ == sizePolicy()) 
        return d.msh_;

    ensurePolished();
    d.valid_hints_ = true;
    // wrap ? golden ratio : min doc size
    d.sh_ = d.sizeForWidth(-1);
    QSize msh_(-1, -1);

    if (!d.isTextLabel_) 
        msh_ = d.sh_;
    else 
    {
        // height for one line
        msh_.rheight() = d.sizeForWidth(QWIDGETSIZE_MAX).height();
        // wrap ? size of biggest word : min doc size
        msh_.rwidth() = d.sizeForWidth(0).width();
        if (d.sh_.height() < msh_.height())
            msh_.rheight() = d.sh_.height();
    }
    const int nHeightTabs = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonTabsHeight, 0, 0);
    d.sh_.setHeight(nHeightTabs-1);
    d.msh_ = msh_;
    d.sizePolicy_ = sizePolicy();
    return msh_;
}

void RibbonTab::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    QTN_D(RibbonTab);
    d.tabMouseOver_ = true;
    update();
}

void RibbonTab::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    QTN_D(RibbonTab);
    d.tabMouseOver_ = false;
    update();
}

void RibbonTab::mousePressEvent(QMouseEvent* event)
{
    QTN_D(RibbonTab);
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    if (RibbonBar* rb = qobject_cast<RibbonBar*>(parentWidget()->parentWidget()))
    {
        if (rb->isBackstageVisible())
        {
            if (RibbonSystemButton* button = rb->getSystemButton())
            {
                if (RibbonBackstageView* backstage = qobject_cast<RibbonBackstageView*>(button->backstage()))
                    backstage->close();
            }
        }
    }

    bool minimized = false;
    if (RibbonBar* rb = qobject_cast<RibbonBar*>(parentWidget()->parentWidget()))
        minimized = rb->isMinimized();

    d.minimizeDblClick_ = d.select_;

    if (d.minimizeDblClick_ && minimized && !d.tracking_)
        d.minimizeDblClick_ = false;

    if (d.minimizeDblClick_)
    {
        event->ignore();
        return;
    }

    emit pressTab(this);

    repaint();
    event->ignore();
}

void RibbonTab::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    if (event->button() != Qt::LeftButton)
        return;
    update();
}

void RibbonTab::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTN_D(RibbonTab);

    if (event->button() != Qt::LeftButton)
    {
        QWidget::mouseDoubleClickEvent(event);
        return;
    }

    if (d.select_ && d.minimizeDblClick_)
    {
        d.minimizeDblClick_ = true;
        QWidget::mouseDoubleClickEvent(event);
    }
}



namespace Qtitan
{
    /* RibbonTabBarPrivate */
    class RibbonTabBarPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonTabBar)

    public:
        explicit RibbonTabBarPrivate();

    public:
        void layoutWidgets();
        bool compressTabs();
        bool validIndex(int index) const { return index >= 0 && index < tabList_.count(); }
        bool validWidth() const;

        RibbonBar* getRibbonBar() const;

    public:
        QList<RibbonTab*> tabList_;
        QList<QWidget*> controls_;
        QList<QWidget*> listEnabledWidgets_;
        RibbonSystemButton* controlSystemButton_;
        QAction* actionSystemPopupBar_;
        bool validRect_;
        int currentIndex_;
        int margin_;
        RibbonTabBar::SelectionBehavior selectionBehaviorOnRemove;
    };
};

RibbonTabBarPrivate::RibbonTabBarPrivate()
    : controlSystemButton_(Q_NULL)
    , actionSystemPopupBar_(Q_NULL)
    , validRect_(true)
    , currentIndex_(-1)
    , margin_(1) 
    , selectionBehaviorOnRemove(RibbonTabBar::SelectRightTab)
{
}

void RibbonTabBarPrivate::layoutWidgets()
{
    QTN_P(RibbonTabBar);
    if (p.geometry().isEmpty())
        return; 

    int leftSide = 0;
    int rightSide = 0;

    // set the position of the system button
    int leftIndentTabs = 0;
    if (controlSystemButton_)
    {
        controlSystemButton_->move(margin_ * 4, margin_ /*+ 1*/);
        leftIndentTabs = controlSystemButton_->sizeHint().width();
        leftIndentTabs += margin_ * 4;
        leftSide = controlSystemButton_->geometry().right();
    }

    int nOffset = 0;
    if (QMenuBar* menuBar = qobject_cast<QMenuBar*>(p.parentWidget())) 
    {
        if (QWidget* rightCorner = menuBar->cornerWidget(Qt::TopRightCorner))
        {
            nOffset = rightCorner->size().width();
            nOffset += 1;
            rightSide = rightCorner->geometry().left();
        }
    }
    // set the position of controls right side
    QSize szSumControls;
    QRect rect = p.rect();
    for (int i = 0, count = controls_.size(); count > i; i++) 
    {
        QWidget * widget = controls_.at(i);
        if (widget->isVisible())
        {
            QSize szControl = widget->sizeHint();
            widget->setGeometry( QRect(QPoint(rect.right() - szControl.width() - szSumControls.width() - nOffset, 0),
                QSize(widget->sizeHint().width(), p.sizeHint().height() - margin_)) );
            szSumControls += widget->sizeHint();
        }
    }

    rightSide = controls_.size() > 0 ? controls_.at(controls_.size()-1)->geometry().left() : 
        rightSide == 0 ? p.geometry().right() : rightSide;

    int sumWidth = 0;
    QList<QRect> rectTabs;
    for (int i = 0, count = tabList_.count(); count > i; i++)
    {
        RibbonTab* tab = tabList_.at(i);
        tab->qtn_d().valid_hints_ = false;
        QRect rect = !tab->isHidden() ? QRect(QPoint(sumWidth + margin_ + leftIndentTabs, 0), tab->sizeHint()) : QRect();
        sumWidth += rect.width();
        rectTabs << rect;
    }

    int newWidth = 0;
    if (!p.geometry().isEmpty() && tabList_.count() > 0)
    {
        int clientWidth = rightSide - leftSide;
        if (clientWidth < sumWidth)
            newWidth = (sumWidth-clientWidth)/tabList_.count();
    }

    bool validRect = true;
    // calculate the width of the tabs
    sumWidth = 0;
    for (int i = 0, count = tabList_.count(); count > i; i++) 
    {
        RibbonTab* tab = tabList_.at(i);

        QRect rectItem = rectTabs.at(i);
        if (newWidth != 0)
            rectItem.setWidth(rectItem.width() - newWidth);

        tab->move(sumWidth + margin_ + leftIndentTabs, rectItem.y());
        tab->resize(rectItem.width(), rectItem.height());
        sumWidth += rectItem.width();

        if (!tab->isHidden() && !tab->validRect())
            validRect = false;
    }
    validRect_ = validRect;
}

bool RibbonTabBarPrivate::validWidth() const
{
    return validRect_;
}

RibbonBar* RibbonTabBarPrivate::getRibbonBar() const
{
    QTN_P(const RibbonTabBar);
    return qobject_cast<RibbonBar*>(p.parentWidget());
}



/* RibbonTabBar */
RibbonTabBar::RibbonTabBar(QWidget* pParent)
    : QWidget(pParent)
{
    QTN_INIT_PRIVATE(RibbonTabBar);
}

RibbonTabBar::~RibbonTabBar()
{
    QTN_FINI_PRIVATE();
}

int RibbonTabBar::addTab(const QString& text) 
{
    return insertTab(-1, text);
}

int RibbonTabBar::addTab(const QIcon& icon, const QString& text)
{
    return insertTab(-1, icon, text);
}

int RibbonTabBar::insertTab(int index, const QString& text)
{
    return insertTab(index, QIcon(), text);
}

int RibbonTabBar::insertTab(int index, const QIcon& icon, const QString& text)
{
    Q_UNUSED(icon);
    QTN_D(RibbonTabBar);
    RibbonTab* pTabLabel = new RibbonTab(text, this);
    connect(pTabLabel, SIGNAL(pressTab(QWidget*)), this, SLOT(activateTab(QWidget*)));

    if (!d.validIndex(index)) 
    {
        index = d.tabList_.count();
        d.tabList_.append(pTabLabel);
    } 
    else 
        d.tabList_.insert(index, pTabLabel);

    if (d.tabList_.count() == 1)
        setCurrentIndex(index);
    else if (index <= d.currentIndex_)
        ++d.currentIndex_;

    for (int i = 0; i < d.tabList_.count(); ++i) 
    {
        if (d.tabList_[i]->lastTab >= index)
            ++d.tabList_[i]->lastTab;
    }

    bool setSelect = true;
    if (RibbonBar* rb = d.getRibbonBar())
        setSelect = !rb->isMinimized();

    if (setSelect)
    {
        for (int i = 0; i < d.tabList_.count(); i++) 
            d.tabList_.at(i)->setSelect(d.currentIndex_ == i);
    }

    if (isVisible())
        pTabLabel->show();

    d.layoutWidgets();
    return index;
}

void RibbonTabBar::moveTab(int index, int newIndex)
{
    QTN_D(RibbonTabBar);
    if (d.validIndex(index) && d.validIndex(newIndex))
    {
        // save pointer active tab
        RibbonTab* saveTab = d.tabList_[d.currentIndex_];
        // Moves the tab in the position index to the position newIndex
        d.tabList_.move(index, newIndex);
        // set an index of active tab
        int ind = d.tabList_.indexOf(saveTab);
        if (d.validIndex(ind) && ind != d.currentIndex_)
            d.currentIndex_ = ind;
        // reposition tabs
        d.layoutWidgets();
    }
}

void RibbonTabBar::removeTab(int index)
{
    QTN_D(RibbonTabBar);
    if (d.validIndex(index)) 
    {
        int newIndex = d.tabList_[index]->lastTab;
        RibbonTab* tab = d.tabList_[index];
        d.tabList_.removeAt(index);
        delete tab;

        for (int i = 0; i < d.tabList_.count(); ++i) 
        {
            if (d.tabList_[i]->lastTab == index)
                d.tabList_[i]->lastTab = -1;
            if (d.tabList_[i]->lastTab > index)
                --d.tabList_[i]->lastTab;
        }
        if (index == d.currentIndex_) 
        {
            // The current tab is going away, in order to make sure
            // we emit that "current has changed", we need to reset this
            // around.
            d.currentIndex_ = -1;
            if (d.tabList_.size() > 0) 
            {
                switch(d.selectionBehaviorOnRemove) 
                {
                case SelectPreviousTab:
                    if (newIndex > index)
                        newIndex--;
                    if (d.validIndex(newIndex))
                        break;
                    // else fallthrough
                case SelectRightTab:
                    newIndex = index;
                    if (newIndex >= d.tabList_.size())
                        newIndex = d.tabList_.size() - 1;
                    break;
                case SelectLeftTab:
                    newIndex = index - 1;
                    if (newIndex < 0)
                        newIndex = 0;
                    break;
                default:
                    break;
                }

                if (d.validIndex(newIndex)) 
                {
                    // don't loose newIndex's old through setCurrentIndex
                    int bump = d.tabList_[newIndex]->lastTab;
                    if (d.getRibbonBar() && !d.getRibbonBar()->isMinimized())
                        setCurrentIndex(newIndex);
                    d.tabList_[newIndex]->lastTab = bump;
                }
            } 
            else 
            {
//                emit currentChanged(-1);
            }
        } 
        else if (index < d.currentIndex_) 
        {
            if (d.getRibbonBar() && !d.getRibbonBar()->isMinimized())
                setCurrentIndex(d.currentIndex_ - 1);
        }
    }
}

int RibbonTabBar::currentIndex() const
{
    QTN_D(const RibbonTabBar);
    if (d.validIndex(d.currentIndex_))
        return d.currentIndex_;
    return -1;
}

RibbonTab* RibbonTabBar::getTab(int nIndex) const
{
    QTN_D(const RibbonTabBar);
    if (d.validIndex(nIndex))
        return d.tabList_[nIndex];
    return Q_NULL;
}

int RibbonTabBar::getTabCount() const
{
    QTN_D(const RibbonTabBar);
    return d.tabList_.count();
}

int RibbonTabBar::getIndex(const RibbonTab* tab) const
{
    for (int i = 0, count = getTabCount(); count > i; i++)
    {
        if (getTab(i) == tab)
            return i;
    }
    return -1;
}

RibbonTabBar::SelectionBehavior RibbonTabBar::selectionBehaviorOnRemove() const
{
    QTN_D(const RibbonTabBar);
    return d.selectionBehaviorOnRemove;
}

void RibbonTabBar::setSelectionBehaviorOnRemove(RibbonTabBar::SelectionBehavior behavior)
{
    QTN_D(RibbonTabBar);
    d.selectionBehaviorOnRemove = behavior;
}

void RibbonTabBar::setEnabledWidgets(bool enabled)
{
    QTN_D(RibbonTabBar);
    if (enabled)
    {
        for (int i = 0, count = d.listEnabledWidgets_.size(); count > i; i++) 
        {
            QWidget * widget = d.listEnabledWidgets_.at(i);
            widget->setEnabled(true);
        }
        d.listEnabledWidgets_.clear();
    }
    else
    {
        d.listEnabledWidgets_.clear();
        for (int i = 0, count = d.controls_.size(); count > i; i++) 
        {
            QWidget * widget = d.controls_.at(i);
            if (widget->isEnabled() /*&& widget->objectName() == QString("No")*/)
            {
                d.listEnabledWidgets_ << widget;
                widget->setEnabled(false);
            }
        }
    }
}

QAction* RibbonTabBar::addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, QMenu* menu)
{
    QTN_D(RibbonTabBar);
    if ( Qt::ToolButtonTextUnderIcon == style )
    {
        Q_ASSERT(false);
        style = Qt::ToolButtonTextBesideIcon;
    }

    QAction* action = new QAction(icon, text, this);
    QWidget::addAction(action);
    if (menu)
        action->setMenu(menu);

    RibbonButton * button = new RibbonButton(this);

    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolButtonStyle(style);

    if (!icon.isNull() && icon.availableSizes().count() > 0)
        button->setIconSize(icon.availableSizes().first());

    action->setIconText(action->text());
    button->setDefaultAction(action);
    d.controls_.append(button);

    button->setVisible(true);
    layoutWidgets();
    return action;
}

QMenu* RibbonTabBar::addMenu(const QString& title)
{
    QTN_D(RibbonTabBar);
    QMenu *menu = new QMenu(title,this);

    RibbonButton * button = new RibbonButton(this);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(menu);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);

    button->setDefaultAction(menu->menuAction());
    d.controls_.append(button);

    button->setVisible(true);
    layoutWidgets();
    return menu;
}

void RibbonTabBar::setCurrentIndex(int index)
{
    QTN_D(RibbonTabBar);

    bool allowSetCurrentIndex = false;
    if (RibbonBar* rb = d.getRibbonBar())
        allowSetCurrentIndex = rb->isMinimized();

    if (d.validIndex(index) && (d.currentIndex_ != index || allowSetCurrentIndex)) 
    {
        d.currentIndex_ = index;

        emit currentChanged(index);

        for (int i = 0; i < d.tabList_.count(); i++) 
            d.tabList_.at(i)->setSelect(index == i);
        repaint();
    }
}

void RibbonTabBar::layoutWidgets()
{
    QTN_D(RibbonTabBar);
    d.layoutWidgets();
}

void RibbonTabBar::refresh()
{
    QTN_D(RibbonTabBar);
    for (int i = 0; i < d.tabList_.count(); i++) 
        d.tabList_.at(i)->qtn_d().updateLabel();
}

bool RibbonTabBar::validWidth() const
{
    QTN_D(const RibbonTabBar);
    return d.validWidth();
}

bool RibbonTabBar::event(QEvent* event)
{
    QTN_D(RibbonTabBar);
    switch(event->type())
    {
        case QEvent::Shortcut :
            {
                QShortcutEvent* se = static_cast<QShortcutEvent *>(event);
                for (int i = 0; i < d.tabList_.count(); ++i) 
                {
                    const RibbonTab* tab = d.tabList_.at(i);
                    if (tab->shortcut() == se->shortcutId()) 
                    {
                        setCurrentIndex(i);
                        return true;
                    }
                }
            }
            break;
        case QEvent::LayoutRequest :
                layoutWidgets();
            break;
        default:
            break;
    }

    return QWidget::event(event);
}

QSize RibbonTabBar::sizeHint() const
{
    const int heightTabs = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonTabsHeight, 0, 0);
    return QSize(0, heightTabs-2).expandedTo(QApplication::globalStrut());
}

void RibbonTabBar::activateTab(QWidget* widget)
{
    QTN_D(RibbonTabBar);

    for (int i = 0; i < d.tabList_.count(); ++i) 
    {
        if (d.tabList_.at(i) == widget) 
        {
            bool allow = true;
            if (RibbonBar* ribbonBar = d.getRibbonBar())
            {
                if (RibbonPage* page = ribbonBar->getPage(i))
                    page->activatingPage(allow);
            }

            if (allow)
                setCurrentIndex(i);
            return;
        }
    }
}
