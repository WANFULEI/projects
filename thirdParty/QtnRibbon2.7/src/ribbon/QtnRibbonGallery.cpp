/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonGallery for Qt)
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
#include <QToolButton>
#include <QAction>
#include <QStyleOption>
#include <QScrollBar>
#include <QToolTip>
#include <QBitmap>
#include <qevent.h>
#include <qdebug.h>
#include <qstyleoption.h>
#include <QDesktopWidget>
#include <qmath.h>

#include "QtnOfficePopupMenu.h"
#include "QtnOfficeStyle.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonGallery.h"
#include "QtnRibbonGalleryPrivate.h"

using namespace Qtitan;


/*!
\class Qtitan::qtn_galleryitem_rect
\internal
*/

/*!
\class Qtitan::WidgetItemData
\internal
*/

/*!
\class Qtitan::RibbonGalleryItemPrivate
\internal
*/
RibbonGalleryItemPrivate::RibbonGalleryItemPrivate()
{
    m_index = -1;
    m_label = false;
    m_enabled = true;
}

void RibbonGalleryItemPrivate::init()
{
    QTN_P(RibbonGalleryItem);
    p.setSizeHint(QSize(0, 0));
}

/*!
\class Qtitan::RibbonGalleryGroupPrivate
\internal
*/
RibbonGalleryGroupPrivate::RibbonGalleryGroupPrivate()
{
    sizeItem_ = QSize(0, 0);
    clipItems_ = true;
}

RibbonGalleryGroupPrivate::~RibbonGalleryGroupPrivate()
{
    for (int i = 0, count = viewWidgets_.size(); count > i; i++)
    {
        if (RibbonGallery* gallery = qobject_cast<RibbonGallery*>(viewWidgets_.at(i)))
            gallery->qtn_d().items_ = Q_NULL; 
    }
}

void RibbonGalleryGroupPrivate::init()
{
}

void RibbonGalleryGroupPrivate::updateIndexes(int start /*= 0*/)
{
    QTN_P(RibbonGalleryGroup);
    for (int i = start; i < arrItems_.size(); i++)
        p.getItem(i)->qtn_d().m_index = i;
}

void RibbonGalleryGroupPrivate::itemsChanged()
{
    for (int i = 0; i < viewWidgets_.size(); i++)
    {
        RibbonGallery* gallery = qobject_cast<RibbonGallery*>(viewWidgets_.at(i));
        if (gallery)
        {
            gallery->qtn_d().selected_ = -1;
            gallery->qtn_d().scrollPos_= 0;
            gallery->qtn_d().layoutItems();

            if (gallery->qtn_d().preview_)
                gallery->qtn_d().preview_ = false;
            gallery->update();
        }
    }
}

void RibbonGalleryGroupPrivate::redrawWidget()
{
    for (int i = 0, count = viewWidgets_.size(); count > i; i++)
        viewWidgets_.at(i)->repaint();
}

void RibbonGalleryGroupPrivate::clear()
{
    RibbonGalleryItem* item = Q_NULL;
    foreach (item, arrItems_)
        delete item;
    arrItems_.clear();
}

/*!
\class Qtitan::RibbonGalleryPrivate
\internal
*/
RibbonGalleryPrivate::RibbonGalleryPrivate()
{
    showBorders_        = false;
    showLabels_         = true;
    hideSelection_      = false;
    keyboardSelected_   = false; 
    preview_            = false;
    pressed_            = false;
    animation_          = false;
    scrollPos_          = 0;
    scrollPosTarget_    = 0;
    totalHeight_        = 0;
    selected_           = -1;
    checkedItem_        = Q_NULL; 
    items_              = Q_NULL;
    ptPressed_          = QPoint(0, 0);
    scrollBar_          = Q_NULL; 
    menuBar_            = Q_NULL;
    buttonPopup_        = Q_NULL;
    buttonScrollUp_     = Q_NULL;
    buttonScrollDown_   = Q_NULL;
    animationStep_      = 0.0;
    timerElapse_        = 0;  
}

RibbonGalleryPrivate::~RibbonGalleryPrivate()
{
    QTN_P(RibbonGallery);
    if (items_ && items_->qtn_d().viewWidgets_.size() > 0)
        items_->qtn_d().viewWidgets_.removeOne(&p);
}

void RibbonGalleryPrivate::init()
{
    QTN_P(RibbonGallery);
    p.setAttribute(Qt::WA_MouseTracking);
    p.setObjectName(QLatin1String("RibbonGallery"));
    p.setProperty(qtn_WidgetGallery, true);
    setScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void RibbonGalleryPrivate::layoutItems()
{
    QTN_P(RibbonGallery);

    if (p.isShowAsButton())
        return;

    QRect rcItems = p.getItemsRect();

    int x = rcItems.left();
    int y = rcItems.top();
    bool bFirstItem = true;

    int nCount = p.getItemCount();

    arrRects_.resize(nCount);
    int nRowHeight = 0;

    for (int i = 0 ; i < nCount; i++)
    {
        RibbonGalleryItem* pItem = p.getItem(i);
        arrRects_[i].item = pItem;
        arrRects_[i].beginRow = false;

        if (pItem->isLabel())
        {
            if (!showLabels_)
            {
                arrRects_[i].rect = QRect(QPoint(0, 0), QPoint(0, 0));
                continue;
            }

            if (!bFirstItem)
                y += nRowHeight;

            QRect rcItem(QPoint(rcItems.left(), y), QPoint(rcItems.right(), y + 19));
            arrRects_[i].rect = rcItem;
            arrRects_[i].beginRow = true;

            y += 20;
            nRowHeight = 0;
            x = rcItems.left();
            bFirstItem = true;
        }
        else
        {
            QSize szItem = pItem->sizeHint();
            if (szItem.width() == 0)
                szItem.setWidth(rcItems.width());

            if (bFirstItem)
                arrRects_[i].beginRow = true;

            if (!bFirstItem && x + szItem.width() > rcItems.right())
            {
                y += nRowHeight;
                x = rcItems.left();
                arrRects_[i].beginRow = true;
                nRowHeight = 0;
            }

            QRect rcItem(QPoint(x, y), szItem);
            arrRects_[i].rect = rcItem;

            nRowHeight = qMax(nRowHeight, szItem.height());

            bFirstItem = false;
            x += szItem.width();
        }
    }

    if (!bFirstItem)
        y += nRowHeight;

    totalHeight_ = y - rcItems.top();

    if (scrollPos_ > totalHeight_ - rcItems.height())
        scrollPos_ = qMax(0, totalHeight_ - rcItems.height());

    setScrollBarValue();
}

void RibbonGalleryPrivate::layoutScrollBar()
{
    QTN_P(RibbonGallery);

    if (scrollBar_)
    {
        QSize size = scrollBar_->sizeHint();

        QRect rectScroll(p.rect());
        rectScroll.setLeft(rectScroll.right()-size.width());

        if (qobject_cast<OfficePopupMenu*>(p.parentWidget()))
            rectScroll.adjust(0, 0, 0, 0 );
        else if (showBorders_ )
            rectScroll.adjust(0, 1, -1, -1 );

        scrollBar_->setGeometry(rectScroll);
    }
    else if (buttonPopup_ && buttonScrollUp_ && buttonScrollDown_)
    {
        QRect rectScroll(p.rect());

        if (showBorders_)
            rectScroll.adjust(1, 0, 0, 0 );

        rectScroll.setLeft(rectScroll.right()-14);

        QRect rectUp(QPoint(rectScroll.left(), rectScroll.top()), QPoint(rectScroll.right(), rectScroll.top() + 20));
        buttonScrollUp_->setGeometry(rectUp);
        QRect rectDown(QPoint(rectScroll.left(), rectUp.bottom()+1), QPoint(rectScroll.right(), rectUp.bottom() + 19));
        buttonScrollDown_->setGeometry(rectDown);
        QRect rectScrollPopup(QPoint(rectScroll.left(), rectDown.bottom()+1), QPoint(rectScroll.right(), rectScroll.bottom()));
        buttonPopup_->setGeometry(rectScrollPopup);
    }
}

void RibbonGalleryPrivate::setScrollBarValue()
{
    if (scrollBar_)
    {
        QTN_P(RibbonGallery);
        QRect rcItems = p.getItemsRect();

        int nRangeMax = qMax(0, (totalHeight_ + 2)- scrollBar_->rect().height());
        scrollBar_->setRange(0, nRangeMax);
        scrollBar_->setPageStep(rcItems.height());
        scrollBar_->setSingleStep(20);
        scrollBar_->setValue(scrollPos_);
    }
    else if (buttonPopup_ && buttonScrollUp_ && buttonScrollDown_)
    {
        buttonScrollUp_->setEnabled(isScrollButtonEnabled(true));
        buttonScrollDown_->setEnabled(isScrollButtonEnabled(false));
    }
}

void RibbonGalleryPrivate::setScrollPos(int scrollPos)
{
    QTN_P(RibbonGallery);
    QRect rcItems = p.getItemsRect();

    if (scrollPos > totalHeight_ - rcItems.height())
        scrollPos = totalHeight_ - rcItems.height();

    if (scrollPos < 0)
        scrollPos = 0;

    if (animation_)
    {
        animation_ = false;
        timerElapse_ = 0;
        scrollTimer_.stop();
        scrollPos_ = -1;
    }

    if (scrollPos_ == scrollPos)
        return;

    scrollPos_ = scrollPos;
    layoutItems();

    repaintItems(NULL, false);
}

void RibbonGalleryPrivate::actionTriggered(int action)
{
    QTN_P(RibbonGallery);
    int y = animation_ ? scrollPosTarget_ : scrollPos_;
    QRect rcItems = p.getItemsRect();

    RibbonGalleryGroup* items = p.galleryGroup();
    if (!items)
        return;

    switch(action)
    {
        case QAbstractSlider::SliderSingleStepAdd :
            y += items->getSize().height();
            break;
        case QAbstractSlider::SliderSingleStepSub :
            y -= items->getSize().height();
            break;
        case QAbstractSlider::SliderPageStepAdd :
            y += rcItems.height();
            break;
        case QAbstractSlider::SliderPageStepSub :
            y -= rcItems.height();
            break;
        case QAbstractSlider::SliderToMinimum :
            y = 0;
            break;
        case QAbstractSlider::SliderToMaximum :
            y = totalHeight_;
            break;
        case QAbstractSlider::SliderMove :
            y = scrollBar_->sliderPosition();
            break;
    }

    if (menuBar_ && (action == QAbstractSlider::SliderSingleStepSub || QAbstractSlider::SliderSingleStepAdd))
        startAnimation(y);
    else
        setScrollPos(y);
}

void RibbonGalleryPrivate::pressedScrollUp()
{
    timerElapse_ = QApplication::doubleClickInterval() * 4 / 5;
    actionTriggered(QAbstractSlider::SliderSingleStepSub);
}

void RibbonGalleryPrivate::pressedScrollDown()
{
    timerElapse_ = QApplication::doubleClickInterval() * 4 / 5;
    actionTriggered(QAbstractSlider::SliderSingleStepAdd);
}

void RibbonGalleryPrivate::setScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    QTN_P(RibbonGallery);

    if (policy == Qt::ScrollBarAlwaysOn)
    {
        delete buttonScrollUp_; buttonScrollUp_ = Q_NULL;
        delete buttonScrollDown_; buttonScrollDown_ = Q_NULL;
        delete buttonPopup_; buttonPopup_ = Q_NULL;
        if (menuBar_)
            menuBar_->removeEventFilter(this);
        menuBar_ = Q_NULL;

        if (!scrollBar_)
        {
            scrollBar_ = new QScrollBar(&p);
            setScrollBarValue();
            connect(scrollBar_, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggered(int)));
        }
    }
    else
    {
        if (scrollBar_)
        {
            disconnect(scrollBar_, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggered(int)));
            delete scrollBar_;
            scrollBar_ = Q_NULL;
        }
    }
}

Qt::ScrollBarPolicy RibbonGalleryPrivate::scrollBarPolicy() const
{
    return scrollBar_ ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff;
}

void RibbonGalleryPrivate::setPopupMenu(OfficePopupMenu* popup)
{
    QTN_P(RibbonGallery);

    setScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    menuBar_ = popup;
    menuBar_->installEventFilter(this);

    buttonScrollUp_ = new QToolButton(&p);
    buttonScrollUp_->setProperty(qtn_ScrollUpButtonGallery, true);
    buttonScrollDown_ = new QToolButton(&p);
    buttonScrollDown_->setProperty(qtn_ScrollDownButtonGallery, true);
    buttonPopup_ = new QToolButton(&p);
    buttonPopup_->setProperty(qtn_PopupButtonGallery, true);

    buttonPopup_->setPopupMode(QToolButton::InstantPopup);
    buttonPopup_->setMenu(menuBar_);
    menuBar_->setWidgetBar(&p);

    connect(buttonScrollUp_, SIGNAL(pressed()), this, SLOT(pressedScrollUp()));
    connect(buttonScrollDown_, SIGNAL(pressed()), this, SLOT(pressedScrollDown()));
}

void RibbonGalleryPrivate::drawItems(QPainter* painter)
{
    QTN_P(RibbonGallery);

    RibbonGalleryGroup* items = p.galleryGroup();
    if (!items)
        return;

    Q_ASSERT(arrRects_.size() == items->getItemCount());

    QRect rcItems = p.getItemsRect();

    painter->setClipRect(rcItems);

    int selected = hideSelection_ ? -1 : selected_;
    for (int i = 0; i < arrRects_.size(); i++)
    {
        const qtn_galleryitem_rect& pos = arrRects_[i];

        QRect rcItem = pos.rect;
        rcItem.translate(0, -scrollPos_);

        if (rcItem.bottom() < rcItems.top())
            continue;

        bool enabled = pos.item->isEnabled() && p.isEnabled();

        if (!rcItem.isEmpty())
            pos.item->draw(painter, &p, rcItem, enabled, selected == i, selected == i && pressed_, isItemChecked(pos.item));

        if (rcItem.top() > rcItems.bottom())
            break;
    }
}

void RibbonGalleryPrivate::repaintItems(QRect* pRect, bool bAnimate)
{
    Q_UNUSED(bAnimate);
    QTN_P(RibbonGallery);
#ifdef Q_OS_MAC
    Q_UNUSED(pRect);
    p.repaint();
#else
    p.repaint(pRect ? *pRect : p.rect());
#endif
}

bool RibbonGalleryPrivate::isItemChecked(RibbonGalleryItem* pItem) const
{
    return checkedItem_ == pItem;
}

bool RibbonGalleryPrivate::isScrollButtonEnabled(bool buttonUp)
{
    QTN_P(RibbonGallery);

    if (!p.isEnabled())
        return false;
    return buttonUp ? scrollPos_ > 0 : scrollPos_ < totalHeight_ - p.getItemsRect().height();
}

int RibbonGalleryPrivate::scrollWidth() const
{
    return scrollBar_ ? scrollBar_->sizeHint().width() : buttonScrollUp_ ? buttonScrollUp_->width() : 0;
}

void RibbonGalleryPrivate::startAnimation(int scrollPos)
{
    QTN_P(RibbonGallery);
    QRect rcItems = p.getItemsRect();

    if (scrollPos > totalHeight_ - rcItems.height())
        scrollPos = totalHeight_ - rcItems.height();

    if (scrollPos < 0)
        scrollPos = 0;

    if (animation_ && scrollPosTarget_ == scrollPos)
        return;

    if (!animation_ && scrollPos_ == scrollPos)
        return;

    animation_ = true;
    scrollPosTarget_ = scrollPos;
    animationStep_ = double(scrollPosTarget_ - scrollPos_) / (timerElapse_ > 200 ? 8.0 : 3.0);

    if (animationStep_ > 0 && animationStep_ < 1) animationStep_ = 1;
    if (animationStep_ < 0 && animationStep_ > -1) animationStep_ = -1;

    int nms = 40;
    scrollTimer_.start(nms, this);
    startAnimate();
}

void RibbonGalleryPrivate::startAnimate()
{
    if (qAbs(scrollPos_ - scrollPosTarget_) > qFabs(animationStep_))
    {
        scrollPos_ = int((double)scrollPos_ + animationStep_);
    }
    else
    {
        animation_ = false;
        scrollPos_ = scrollPosTarget_;
        timerElapse_ = 0;
        scrollTimer_.stop();
    }
    layoutItems();
    setScrollBarValue();
    repaintItems(Q_NULL, false);
}

bool RibbonGalleryPrivate::event(QEvent* event)
{
    switch (event->type()) 
    {
        case QEvent::Timer:
            {
                QTimerEvent* timerEvent = (QTimerEvent*)event;
                if (scrollTimer_.timerId() == timerEvent->timerId()) 
                {
                    startAnimate();
                    event->accept();
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return QObject::event(event);
}

bool RibbonGalleryPrivate::eventFilter(QObject* object, QEvent* event)
{
    bool bResult = QObject::eventFilter(object, event);

    if (event->type() == QEvent::Show)
    {
        if (OfficePopupMenu* popup = qobject_cast<OfficePopupMenu*>(object))
        {
            QTN_P(RibbonGallery);
            QPoint pos;
            QRect screen = QApplication::desktop()->availableGeometry(buttonPopup_);
            QSize sh = popup->sizeHint();
            QRect rect = p.rect();
            if (p.mapToGlobal(QPoint(0, rect.bottom())).y() + sh.height() <= screen.height())
                pos = p.mapToGlobal(rect.topLeft());
            else
                pos = p.mapToGlobal(rect.topLeft() - QPoint(0, sh.height()));
            popup->move(pos.x(), pos.y());
        }
    }
    else if (event->type() == QEvent::Hide)
    {
/*
        if (OfficePopupMenu* popup = qobject_cast<OfficePopupMenu*>(object))
        {
            popup->setTearOffEnabled(true);
            popup->setGeometry(QRect(QPoint(0,0), QSize(0, 0)));
            popup->resize(QSize(0,0));
        }
*/
    }
    return bResult;
}



/* RibbonGalleryItem */
RibbonGalleryItem::RibbonGalleryItem()
{
    QTN_INIT_PRIVATE(RibbonGalleryItem);
    QTN_D(RibbonGalleryItem);
    d.init();
}

RibbonGalleryItem::~RibbonGalleryItem()
{
    QTN_FINI_PRIVATE();
}

QSize RibbonGalleryItem::sizeHint() const
{
    QTN_D(const RibbonGalleryItem);
    QSize size = qvariant_cast<QSize>(data(Qt::SizeHintRole));
    return QSize(size.width() == 0 ? d.m_items->qtn_d().sizeItem_.width() : size.width(),
        size.height() == 0 ? d.m_items->qtn_d().sizeItem_.height() : size.height());
}

void RibbonGalleryItem::setSizeHint(const QSize& szItem)
{
    setData(Qt::SizeHintRole, szItem);
}

QIcon RibbonGalleryItem::icon() const
{
    return qvariant_cast<QIcon>(data(Qt::DecorationRole));
}

void RibbonGalleryItem::setIcon(const QIcon& icon)
{
    setData(Qt::DecorationRole, icon);
}

QString RibbonGalleryItem::caption() const
{ 
    return data(Qt::DisplayRole).toString(); 
}

void RibbonGalleryItem::setCaption(const QString& caption)
{
    setData(Qt::DisplayRole, caption);
}

QString RibbonGalleryItem::toolTip() const
{
    return data(Qt::ToolTipRole).toString();
}

void RibbonGalleryItem::setToolTip(const QString& toolTip)
{
    setData(Qt::ToolTipRole, toolTip);
}

QString RibbonGalleryItem::statusTip() const
{ 
    return data(Qt::StatusTipRole).toString(); 
}

void RibbonGalleryItem::setStatusTip(const QString& statusTip)
{
    setData(Qt::StatusTipRole, statusTip);
}

int RibbonGalleryItem::getIndex() const
{
    QTN_D(const RibbonGalleryItem);
    return d.m_index;
}

bool RibbonGalleryItem::isLabel() const
{
    QTN_D(const RibbonGalleryItem);
    return d.m_label;
}

void RibbonGalleryItem::setLabel(bool label)
{
    QTN_D(RibbonGalleryItem);
    d.m_label = label;
}

void RibbonGalleryItem::setEnabled(bool enabled)
{
    QTN_D(RibbonGalleryItem);
    if (d.m_enabled == enabled)
        return;

    d.m_enabled = enabled;

    if (d.m_items)
        d.m_items->qtn_d().redrawWidget();
}

bool RibbonGalleryItem::isEnabled() const
{
    QTN_D(const RibbonGalleryItem);
    return d.m_enabled;
}

void RibbonGalleryItem::setData(int role, const QVariant& value)
{
    QTN_D(RibbonGalleryItem);
    bool found = false;
    role = (role == Qt::EditRole ? Qt::DisplayRole : role);
    for (int i = 0; i < d.values.count(); ++i) 
    {
        if (d.values.at(i).role == role) 
        {
            if (d.values.at(i).value == value)
                return;
            d.values[i].value = value;
            found = true;
            break;
        }
    }
    if (!found)
        d.values.append(WidgetItemData(role, value));

//    if (QListModel *model = (view ? qobject_cast<QListModel*>(view->model()) : 0))
//        model->itemChanged(this);
}

QVariant RibbonGalleryItem::data(int role) const
{
    QTN_D(const RibbonGalleryItem);
    role = (role == Qt::EditRole ? Qt::DisplayRole : role);
    for (int i = 0; i < d.values.count(); ++i)
        if (d.values.at(i).role == role)
            return d.values.at(i).value;
    return QVariant();
}

void RibbonGalleryItem::draw(QPainter* p, RibbonGallery* gallery, QRect rectItem, bool enabled, bool selected, bool pressed, bool checked)
{
    Q_UNUSED(gallery);
//    QTN_D(RibbonGalleryItem);
    if (OfficeStyle* style = qobject_cast<OfficeStyle*>(gallery->style()) )
    {
        if (isLabel())
        {
            style->drawLabelGallery(p, this, rectItem);
            return;
        }

        if (!icon().isNull())
        {
            style->drawRectangle(p, rectItem, selected, pressed, enabled, checked, false, TypeNormal, BarPopup);
            icon().paint(p, rectItem, Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled);
        }
        else
        {
            style->drawRectangle(p, rectItem, selected, false, enabled, checked, false, TypePopup, BarPopup);
            QRect rcText(rectItem);
            rcText.setLeft(rcText.left()+3);
            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter | Qt::TextHideMnemonic;
            QPalette palette;
            palette.setColor(QPalette::WindowText, style->getTextColor(selected, false, enabled, checked, false, TypePopup, BarPopup));
            style->drawItemText(p, rcText, alignment, palette, enabled, caption(), QPalette::WindowText);
        }
    }
}


/* RibbonGalleryGroup */
RibbonGalleryGroup::RibbonGalleryGroup(QObject* parent)
    : QObject(parent)
{
    setObjectName(QLatin1String("RibbonGalleryGroup"));
    QTN_INIT_PRIVATE(RibbonGalleryGroup);
    QTN_D(RibbonGalleryGroup);

    d.init();
}

RibbonGalleryGroup::~RibbonGalleryGroup()
{
    clear();
    QTN_FINI_PRIVATE();
}

RibbonGalleryGroup* RibbonGalleryGroup::createGalleryGroup()
{
    RibbonGalleryGroup* pItems = NULL;
    pItems = new RibbonGalleryGroup();
    return pItems;
}

RibbonGalleryItem* RibbonGalleryGroup::addItem(int image, const QPixmap& pixmap, const QSize& sizeImage, QColor clrTransparent)
{
    QRect rcImage(image*sizeImage.width(), 0, sizeImage.width(), sizeImage.height());
    QPixmap copyPix = pixmap.copy(rcImage.left(), rcImage.top(), rcImage.width(), rcImage.height());

    if (clrTransparent.isValid())
        copyPix.setAlphaChannel(QBitmap(copyPix.createMaskFromColor(clrTransparent, Qt::MaskOutColor)));

    Q_ASSERT(!copyPix.isNull());

    if (copyPix.isNull())
        return Q_NULL;

    return addItem(new RibbonGalleryItem(), QString(), QIcon(copyPix));
}

RibbonGalleryItem* RibbonGalleryGroup::addItem(RibbonGalleryItem* item, const QString& caption, const QIcon& icon)
{
    return insertItem(getItemCount(), item, caption, icon);
}

RibbonGalleryItem* RibbonGalleryGroup::insertItem(int index, RibbonGalleryItem* item, const QString& caption, const QIcon& icon)
{
    QTN_D(RibbonGalleryGroup);

    if (!item)
        return NULL;

    if (index < 0 || index > getItemCount())
        index = getItemCount();

    d.arrItems_.append(item);
    d.updateIndexes(index);

    item->qtn_d().m_items = this;

    if (!icon.isNull())
        item->setIcon(icon);

    if (!caption.isEmpty())
    {
        item->setCaption(caption);
        item->setToolTip(caption);
    }

    d.itemsChanged();
    return item;
}

RibbonGalleryItem* RibbonGalleryGroup::addLabel(const QString& caption)
{
    RibbonGalleryItem* item = addItem(new RibbonGalleryItem());
    item->setCaption(caption);
    item->setLabel(true);
    return item;
}

void RibbonGalleryGroup::clear()
{
    QTN_D(RibbonGalleryGroup);
    d.clear();
    d.itemsChanged();
}

void RibbonGalleryGroup::remove(int index)
{
    QTN_D(RibbonGalleryGroup);

    RibbonGalleryItem* item = getItem(index);
    if (!item)
        return;

    d.arrItems_.remove(index);
    delete item;

    d.itemsChanged();
}

void RibbonGalleryGroup::setSize(const QSize& szItem)
{
    QTN_D(RibbonGalleryGroup);
    d.sizeItem_ = szItem;
}

QSize RibbonGalleryGroup::getSize() const
{
    QTN_D(const RibbonGalleryGroup);
    return d.sizeItem_;
}

int RibbonGalleryGroup::getItemCount() const
{
    QTN_D(const RibbonGalleryGroup);
    return d.arrItems_.size();
}

RibbonGalleryItem* RibbonGalleryGroup::getItem(int index) const
{
    QTN_D(const RibbonGalleryGroup);
    return index >= 0 && index < getItemCount() ? d.arrItems_.at(index) : Q_NULL;
}

void RibbonGalleryGroup::setClipItems(bool clipItems)
{
    QTN_D(RibbonGalleryGroup);
    d.clipItems_ = clipItems;
}


/* RibbonGallery */
RibbonGallery::RibbonGallery(QWidget* parent)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonGallery);
    QTN_D(RibbonGallery);
    d.init();
}

RibbonGallery::~RibbonGallery()
{
    QTN_FINI_PRIVATE();
}

void RibbonGallery::setGalleryGroup(RibbonGalleryGroup* items)
{
    QTN_D(RibbonGallery);
    if (d.items_)
    {
        d.arrRects_.clear();
        d.items_->qtn_d().viewWidgets_.removeOne(this);
        d.items_ = Q_NULL;
    }

    if (items)
    {
        d.items_ = items;
        d.items_->qtn_d().viewWidgets_.append(this);
    }
    d.layoutItems();
    update();
}

void RibbonGallery::setBorderVisible(bool bShowBorders)
{
    QTN_D(RibbonGallery);
    d.showBorders_ = bShowBorders;
    update();
}

bool RibbonGallery::isBorderVisible() const
{
    QTN_D(const RibbonGallery);
    return d.showBorders_;
}

void RibbonGallery::setLabelsVisible(bool showLabels)
{
    QTN_D(RibbonGallery);
    d.showLabels_ = showLabels;
}

bool RibbonGallery::isLabelsVisible() const
{
    QTN_D(const RibbonGallery);
    return d.showLabels_;
}

void RibbonGallery::setScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    QTN_D(RibbonGallery);
    d.setScrollBarPolicy(policy);
}

Qt::ScrollBarPolicy RibbonGallery::scrollBarPolicy() const
{
    QTN_D(const RibbonGallery);
    return d.scrollBarPolicy();
}

void RibbonGallery::setPopupMenu(OfficePopupMenu* popupMenu)
{
    QTN_D(RibbonGallery);
    return d.setPopupMenu(popupMenu);
}

OfficePopupMenu* RibbonGallery::getPopupMenu() const
{
    QTN_D(const RibbonGallery);
    return d.menuBar_;
}

int RibbonGallery::getItemCount() const
{
    RibbonGalleryGroup* pItems = galleryGroup();
    return pItems ? pItems->getItemCount() : 0;
}

RibbonGalleryItem* RibbonGallery::getItem(int nIndex) const
{
    RibbonGalleryGroup* pItems = galleryGroup();
    return pItems ? pItems->getItem(nIndex) : NULL;
}

RibbonGalleryGroup* RibbonGallery::galleryGroup() const
{
    QTN_D(const RibbonGallery);
    return d.items_;
}

void RibbonGallery::setSelectedItem(int indexItem)
{
    QTN_D(RibbonGallery);

    int indSelected = d.selected_;

    d.hideSelection_ = false;
    d.selected_ = indexItem;
    d.pressed_ = false;
    d.keyboardSelected_ = true;

    RibbonGalleryGroup* items = galleryGroup();

    if (items && items->qtn_d().clipItems_ && indSelected != -1)
    {
        QRect rect(getItemDrawRect(indSelected));
        d.repaintItems(&rect, true);
    }

    if (items && items->qtn_d().clipItems_ && d.selected_ != -1)
    {
        QRect rect(getItemDrawRect(d.selected_));
        d.repaintItems(&rect, false);
    }

    if (!items || !items->qtn_d().clipItems_)
        d.repaintItems();

    if (d.selected_ != -1)
    {
    }
    if (d.selected_ != -1 && !d.preview_)
        d.preview_ = true;

    if (d.preview_)
    {
    }

    if (d.selected_ == -1 && d.preview_)
        d.preview_ = false;
    selectedItemChanged();
}

int RibbonGallery::getSelectedItem() const
{
    QTN_D(const RibbonGallery);
    return d.selected_;
}

void RibbonGallery::setCheckedIndex(int index)
{
    QTN_D(RibbonGallery);

    RibbonGalleryItem* previous = NULL;
    int previousInd = getCheckedIndex();
    if (previousInd != -1)
        previous = getItem(previousInd);

    RibbonGalleryItem* currentItem = getItem(index);
    if (currentItem && d.checkedItem_ != currentItem)
    {
        d.checkedItem_ = currentItem;
        d.repaintItems();
        emit currentItemChanged(currentItem, previous);
    }
}

int RibbonGallery::getCheckedIndex() const
{
    QTN_D(const RibbonGallery);
    if (d.checkedItem_)
        return d.checkedItem_->getIndex();
    return -1;
}

void RibbonGallery::setCheckedItem(const RibbonGalleryItem* item)
{
    Q_ASSERT(item != Q_NULL);
    if (item->getIndex() != -1)
        setCheckedIndex(item->getIndex());
}

RibbonGalleryItem* RibbonGallery::getCheckedItem() const
{
    QTN_D(const RibbonGallery);
    return d.checkedItem_;
}

bool RibbonGallery::isShowAsButton() const
{
    return false;
}

bool RibbonGallery::isItemSelected() const
{
    QTN_D(const RibbonGallery);
    return !d.hideSelection_ && getSelectedItem() != -1;
}

QRect RibbonGallery::getBorders() const
{
    QTN_D(const RibbonGallery);
    QRect rcBorders(0, 0, 0, 0);

    if (d.showBorders_)
    {
        rcBorders.setTopLeft(QPoint(1,1));
        rcBorders.setBottomRight(QPoint(1,1));
    }

//  if (d.scrollBar_ && !d.scrollBar_->isHidden())
        rcBorders.setRight(rcBorders.right() + d.scrollWidth()/*scrollBar_->sizeHint().width()*/);

/*
    if (hasBottomSeparator())
        rcBorders.setBottom(2);
*/
    int left = 0, top = 0, right = 0, bottom = 0;
    getContentsMargins(&left, &top, &right, &bottom);
    rcBorders.adjust(left, top, right, bottom);

    return rcBorders;
}

QRect RibbonGallery::getItemDrawRect(int nIndex)
{
    QTN_D(RibbonGallery);
    if (nIndex < 0 || nIndex >= d.arrRects_.size())
        return QRect(0, 0, 0, 0);

    QRect rc(d.arrRects_[nIndex].rect);
    rc.translate(0, -d.scrollPos_);

    rc = rc.intersect(getItemsRect());
    return rc;
}

bool RibbonGallery::isResizable() const
{
    bool isResizable = sizePolicy().horizontalPolicy() == QSizePolicy::Expanding || 
        sizePolicy().horizontalPolicy() == QSizePolicy::MinimumExpanding;
    return !isShowAsButton() && galleryGroup() ? isResizable : false;
}

int RibbonGallery::hitTestItem(QPoint point, QRect* pRect) const
{
    if (!isEnabled())
        return -1;

    if (isShowAsButton())
        return -1;

    QTN_D(const RibbonGallery);
    Q_ASSERT(d.arrRects_.size() == getItemCount());

    QRect rcItems = getItemsRect();

    for (int i = 0; i < d.arrRects_.size(); i++)
    {
        const qtn_galleryitem_rect& pos = d.arrRects_[i];

        QRect rcItem = pos.rect;
        rcItem.translate(0, -d.scrollPos_);

        if (!pos.item->isEnabled())
            continue;

        if (rcItem.top() >= rcItems.bottom())
            break;

        if (rcItem.contains(point))
        {
            if (pRect)
                *pRect = rcItem;
            return pos.item->isLabel() ? -1 : i;
        }
    }

    return -1;
}

QRect RibbonGallery::getItemsRect() const
{
    QRect rc = rect();
    QRect rcBorders = getBorders();
    rc.adjust(rcBorders.left(), rcBorders.top(), -rcBorders.right(), -rcBorders.bottom());
    return rc;
}

void RibbonGallery::hideSelection()
{
    QTN_D(RibbonGallery);
    d.hideSelection_ = true;
    d.keyboardSelected_ = false;

    if (d.selected_ != -1)
    {
        RibbonGalleryGroup* pItems = galleryGroup();
        if (pItems && pItems->qtn_d().clipItems_)
        {
            QRect rect(getItemDrawRect(d.selected_));
            d.repaintItems(&rect, false);
        }
        else
            d.repaintItems();
    }

    if (d.preview_ && d.selected_ != -1)
    {
    }

    if (d.preview_)
    {
        d.preview_ = false;
    }
    selectedItemChanged();
}

void RibbonGallery::selectedItemChanged()
{
}

QSize RibbonGallery::sizeHint() const
{
    int widthGallery = qMax(isResizable() ? minimumSizeHint().width() : 0, baseSize().width());
    int heightGallery = baseSize().height();

    if (OfficePopupMenu* popupBar = qobject_cast<OfficePopupMenu*>(parentWidget()))
        widthGallery = popupBar->width()-2;

    return QSize(widthGallery, heightGallery);
}

QSize RibbonGallery::minimumSizeHint() const
{
    if (!parentWidget() || !galleryGroup())
        return QSize(0, 0);

    QRect rcBorders(getBorders());
    int width = qMax(16, galleryGroup()->qtn_d().sizeItem_.width()) + rcBorders.left() + rcBorders.right();

    int height = qMax(galleryGroup()->qtn_d().sizeItem_.height() + rcBorders.top() + rcBorders.bottom(), 32);
    return QSize(width, height);
}

void RibbonGallery::paintEvent(QPaintEvent*)
{
    QTN_D(RibbonGallery);
    QPainter p(this);

    QStyleOption opt;
    opt.init(this);

    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonFillRect, &opt, &p, this);

    if (!parentWidget()->property(qtn_PopupBar).toBool() && isBorderVisible())
    {
        opt.rect.adjust(0, 0, -1, -1);
        style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonRect, &opt, &p, this);
    }

    d.drawItems(&p);
}

void RibbonGallery::mousePressEvent(QMouseEvent* event)
{
    QTN_D(RibbonGallery);

    if (isShowAsButton())
    {
        QWidget::mousePressEvent(event);
        return;
    }

    if (d.selected_ != -1 && !d.keyboardSelected_)
    {
        d.pressed_ = true;
        d.ptPressed_ = event->pos();
        d.repaintItems();

        int index = hitTestItem(event->pos());
        if (index != -1)
            emit itemPressed(getItem(index));
    }
}

void RibbonGallery::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(RibbonGallery);
    if (d.pressed_ || isItemSelected())
    {
        d.pressed_ = false;
        d.repaintItems();

        int index = hitTestItem(event->pos());
        if (index != -1)
        {
            d.repaintItems();
            emit itemSelectionChanged();
            RibbonGalleryItem* item = getItem(index);

            emit itemClicked(item);

            bool handled = true;
            emit itemClicking(item, handled);

            if (handled)
            {
                if (OfficePopupMenu* popupBar = qobject_cast<OfficePopupMenu*>(parentWidget()))
                    popupBar->close();
            }
        }
    }
}

void RibbonGallery::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(RibbonGallery);

    if (isShowAsButton())
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    int nItem = hitTestItem(event->pos());

    if (nItem == -1 && d.keyboardSelected_)
        return;

    if (nItem == -1 && event->pos() == QPoint(-1, -1) && d.selected_ != -1)
    {
        hideSelection();
        return;
    }

    int nSelected = d.selected_;
    if (nItem != d.selected_ || d.hideSelection_ || d.keyboardSelected_)
    {
        d.selected_ = nItem;
        d.hideSelection_ = false;
        d.keyboardSelected_ = false;
        d.pressed_ = false;

        RibbonGalleryGroup* pItems = galleryGroup();

        if (pItems && pItems->qtn_d().clipItems_)
        {
            if (nSelected != -1)
            {
                QRect rect(getItemDrawRect(nSelected));
                d.repaintItems(&rect, true);
            }

            if (d.selected_ != -1)
            {
                QRect rect(getItemDrawRect(d.selected_));
                d.repaintItems(&rect, false);
            }
        }
        else
            d.repaintItems();

        if (!d.preview_ && d.selected_ != -1)
        {
        }

        if (d.preview_)
        {
        }

        if (d.selected_ == -1 && d.preview_)
        {
            d.preview_ = false;
        }
        selectedItemChanged();
    }
    if (d.pressed_ && (d.ptPressed_ != QPoint(0, 0)) && (d.selected_ != -1) &&
        (abs(d.ptPressed_.x() - event->pos().x()) > 4 || abs(d.ptPressed_.y() - event->pos().y()) > 4))
    {
        d.ptPressed_ = QPoint(0, 0);
    }
}

void RibbonGallery::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    QTN_D(RibbonGallery);
    if (d.selected_ != -1)
        hideSelection();
}

void RibbonGallery::focusOutEvent(QFocusEvent* event)
{
    QWidget::focusOutEvent(event);
}

void RibbonGallery::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    QTN_D(RibbonGallery);
    d.layoutItems();
    d.layoutScrollBar();
}
