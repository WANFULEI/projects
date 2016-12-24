/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonGroup for Qt)
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
#include <QWidgetAction>
#include <QStyleOption>
#include <QResizeEvent>
#include <QPainter>
#include <QAction>
#include <QLineEdit>
#include <QComboBox>
#include <QLayout>
#include <QSpinBox>
#include <QListView>
#include <qdebug.h>

#include "QtnRibbonGroup.h"
#include "QtnRibbonGallery.h"
#include "QtnRibbonPrivate.h"
#include "QtnOfficePopupMenu.h"
#include "QtnRibbonGroupPrivate.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonButton.h"

using namespace Qtitan;

#define ADDITIVE_WIDTH 20

/*!
\class Qtitan::GroupItem
\internal
*/
GroupItem::GroupItem(QWidget* widget, bool begin, bool group)
    : QWidgetItem(widget)
    , action(Q_NULL)
    , tailWidth(0)
    , customWidget(false)
    , beginGroup(begin)
    , wrapGroup(false)
    , wrap(false)
{
    wrapGroup = beginGroup;
    widget->setProperty(qtn_Prop_Begin_Group, beginGroup);
    widget->setProperty(qtn_Prop_Wrap, wrap);
    widget->setProperty(qtn_Prop_Group, group);
}

bool GroupItem::isEmpty() const
{
    return action == 0 || !action->isVisible();
}

/*!
\class Qtitan::GroupLayout
\internal
*/
GroupLayout::GroupLayout(QWidget* parent)
    : QLayout(parent)
    , dirty(true)
    , hasSeparator(false)
{
    width_ = 0;
    rowCount_ = 2;
    if (!qobject_cast<RibbonGroup*>(parent))
        return;
}

GroupLayout::~GroupLayout()
{
    while (!items.isEmpty())
    {
        GroupItem* item = items.takeFirst();
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(item->action)) 
        {
            if (item->customWidget)
                widgetAction->releaseWidget(item->widget());
        }
        delete item;
    }
}

void GroupLayout::updateMarginAndSpacing()
{
    if (!qobject_cast<RibbonGroup*>(parentWidget()))
        return;
    setMargin( 2 );
    setSpacing( 0 );
}

QSize GroupLayout::sizeRealHint() const
{
    if ( dirty)
        const_cast<GroupLayout*>(this)->layoutActions();
    return hint;
}

int GroupLayout::getMinimumWidth() const
{
    int width = 0;
    RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return width;

    const QString& txt = rg->title();
    if (!txt.isEmpty())
    {
        QFontMetrics fm = rg->fontMetrics();
        QSize textSize = fm.size(Qt::TextShowMnemonic, txt);
        textSize.setWidth(textSize.width() + fm.width(QLatin1Char(' '))*2);
        width = textSize.width() + (rg->isOptionButtonVisible() ? 0 : ADDITIVE_WIDTH-4);
    }

    if (rg->isOptionButtonVisible())
        width += rg->qtn_d().butOption_->width();

    return width;
}

QWidget* GroupLayout::getNextWidget(const QWidget* w) const
{
    int index = getNextIndex(w);
    return index != -1 ? items.at(index)->widget() : Q_NULL;
}

QWidget* GroupLayout::getWidget(int index) const
{
    if (index < 0 || index >= items.count())
        return Q_NULL;
    GroupItem* item = items.at(index);
    return item->widget();
}

int GroupLayout::getIndexWidget(const QWidget* w) const
{
    for (int i = 0; i < items.count(); ++i) 
    {
        GroupItem * item = (GroupItem*)items.at(i);
        if (item->widget() == w)
            return i;
    }
    return -1;
}

int GroupLayout::getNextIndex(const QWidget* w) const
{
    int nIndex = indexOf((QWidget*)w);
    int nNextIndex = nIndex + 1;
    if (nNextIndex >= 0 && nNextIndex < items.count())
    {
        GroupItem* item = (GroupItem*)itemAt(nNextIndex);
        if (item && !item->action->isSeparator())
            return nNextIndex;
    }
    return nIndex;
}


void GroupLayout::addItem(QLayoutItem*)
{
    qWarning() << "GroupLayout::addItem(): please use addAction() instead";
    return;
}

QLayoutItem* GroupLayout::itemAt(int index) const
{
    if (index < 0 || index >= items.count())
        return 0;
    return items.at(index);
}

QLayoutItem* GroupLayout::takeAt(int index)
{
    if (index < 0 || index >= items.count())
        return 0;

    GroupItem* item = items.takeAt(index);
    QWidgetAction *widgetAction = qobject_cast<QWidgetAction*>(item->action);
    if (widgetAction != 0 && item->customWidget) 
    {
        widgetAction->releaseWidget(item->widget());
    } 
    else 
    {
        // destroy the QToolButton/QToolBarSeparator
        item->widget()->hide();
        item->widget()->deleteLater();
    }
    invalidate();
    return item;
}

void GroupLayout::insertAction(int index, QAction* action)
{
    index = qMax( 0, index );
    index = qMin( items.count(), index );

    GroupItem *item = createItem(action);
    if (item) 
    {
        items.insert(index, item);
        invalidate();
    }
}

int GroupLayout::indexOf(QAction* action) const
{
    for (int i = 0; i < items.count(); ++i) 
    {
        if (items.at(i)->action == action)
            return i;
    }
    return -1;
}

int GroupLayout::count() const
{
    return items.count();
}

void GroupLayout::invalidate()
{
    dirty = true;
    QLayout::invalidate();
}

void GroupLayout::setGeometry(const QRect& rect)
{
    dirty = false;
    QLayout::setGeometry(rect);
    layoutActions();
}

void GroupLayout::layoutActions()
{
    if (!dirty)
        updateGeomArray();

    QList<QWidget*> showWidgets, hideWidgets;

    for (int i = 0; i < items.count(); i++)
    {
        GroupItem* item = items.at(i);

        if (item->isEmpty())
            hideWidgets << item->widget();
        else
        {
            item->setGeometry(item->rcItem);
            showWidgets << item->widget();
        }
    }

    // we have to do the show/hide here, because it triggers more calls to setGeometry :(
    for (int i = 0; i < showWidgets.count(); ++i)
        showWidgets.at(i)->show();
    for (int i = 0; i < hideWidgets.count(); ++i)
        hideWidgets.at(i)->hide();
}

void GroupLayout::updateGeomArray() const
{
    GroupLayout* that = const_cast<GroupLayout*>(this);
    that->minSize = QSize(0, 0);
    that->hint = QSize(0, 0);

    RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return;

    if (rg->isControlsGrouping())
    {
        updateGeomArray_();
        return;
    }

    const int margin = this->margin();
    const int heightGroup = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);
    int height = heightGroup/3;

    QSize sizeResult(0, heightGroup);

    int resultHeight = heightGroup + 2;
    int dyPrev = 0, dxPrev = 0, x = 2;

    QList<ExWidgetWrapper*> alignWrappers;
    int maxHeight = heightGroup;
    for (int i = 0; i < items.count(); i++)
    {
        GroupItem* item = items.at(i);

        // if the action are invisible
        bool empty = item->isEmpty();
        if (empty)
            continue;

        bool fullRow = false;

        QSize szControl = item->szItem;
        if (szControl.isEmpty())
        {
            QSize szBase = item->widget()->baseSize();
            item->szItem = szBase.isNull() ? item->sizeHint() : szBase;
            szControl = item->szItem;
        }

        maxHeight = qMax(maxHeight, szControl.height());

        int dx = szControl.width();
        int dy = qMax(height, szControl.height());

        bool beginGroup = item->beginGroup && i > 0;
        bool wrapGroup  = item->wrapGroup  && i > 0;

        if (beginGroup)
            x += 6;

        if (i > 0 && !wrapGroup && !fullRow && dyPrev + dy <= resultHeight)
        {
            item->rcItem = QRect(QPoint(x, dyPrev + 1), QSize(dx, dy));
            dxPrev = qMax(dxPrev, dx);

            if (ExWidgetWrapper* wrapper = qobject_cast<ExWidgetWrapper*>(item->widget()))
            {
                if (wrapper->alignWidget())
                    alignWrappers << wrapper;
            }
        }
        else
        {
            updateAlignWidget(alignWrappers);
            alignWrappers.clear();

            x += dxPrev;
            item->rcItem = QRect(QPoint(x, 0), QSize(dx, dy));
            dxPrev = dx;

            if (ExWidgetWrapper* wrapper = qobject_cast<ExWidgetWrapper*>(item->widget()))
            {
                if (wrapper->alignWidget())
                    alignWrappers << wrapper;
            }
        }

        sizeResult.setWidth(qMax(x + dx, sizeResult.width()));
        dyPrev = item->rcItem.bottom();
    }

    updateAlignWidget(alignWrappers);
    alignWrappers.clear();

    that->width_ = sizeResult.width() + 2;

    sizeResult.setWidth(sizeResult.width() + 4*margin);
    that->minSize = sizeResult;

    updateBorders(qMax(that->width_, getMinimumWidth()), QRect());

    that->hint = sizeResult;
    that->hint.setHeight(maxHeight);
    that->dirty = false;
}

void GroupLayout::updateAlignWidget(QList<ExWidgetWrapper*>& alignWrappers) const
{
    int count = alignWrappers.count();
    if (count > 1)
    {
        int maxWidth = 0;
        for (int i = 0; i < count; i++)
        {
            ExWidgetWrapper* wrapper = alignWrappers.at(i);
            QFontMetrics fm = wrapper->fontMetrics();
            QSize sz = fm.size(Qt::TextHideMnemonic, wrapper->labelText());
            maxWidth = qMax(maxWidth, sz.width() + fm.width(QLatin1Char('x')));
        }
        for (int i = 0; i < count; i++)
            alignWrappers.at(i)->setLengthLabel(maxWidth);
    }
}

void GroupLayout::updateBorders(int width, QRect rcBorder) const
{
    RibbonGroup * rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return;

    const int margin = this->margin();
    int groupHeight = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);
    int groupClientHeight = groupHeight;

    bool firstItem = true;
    int numFirstItem = 0;
    int column = 0;
    bool centerItems = rg->isControlsCentering();
    if (rg->isControlsGrouping())
        centerItems = false;

    int controlCount = items.count();

    int leftOffset = margin;

    int minWidth = getMinimumWidth();
    if (rg->isControlsCentering() && width_ < minWidth && width == minWidth)
        leftOffset = 2 + (minWidth - width_) / 2;

    for (int i = 0; i < controlCount; i++)
    {
        GroupItem* item = items.at(i);

        QRect rcControl = item->rcItem;
        rcControl.translate(rcBorder.left() + leftOffset, rcBorder.top() + margin/* + 1*/);
        item->rcItem = rcControl;

        item->rcRow = QRect(QPoint(rcBorder.left(), rcBorder.top()), 
            QPoint(rcBorder.left() + width, rcBorder.top() + groupClientHeight));

        if (centerItems && !firstItem)
        {
            if (column != rcControl.left())
            {
                updateCenterColumn(numFirstItem, i - 1, groupClientHeight - 3);
                column = rcControl.left();
                numFirstItem = i;
            }
        }
        else
        {
            firstItem = false;
            column = rcControl.left();
        }
    }
    if (centerItems && controlCount > 0) 
        updateCenterColumn(numFirstItem, controlCount - 1, groupClientHeight - 3);

    //    int beginGroup = rcBorder.left();
    //    int endGroup = beginGroup + width + 5;

    //    setRect(QRect(QPoint(beginGroup, rcBorder.top), QPoint(endGroup,  rcBorder.top + groupClientHeight)));
}

void GroupLayout::updateCenterColumn(int firstItem, int lastItem, int groupHeight) const
{
    int nHeight = 0;
    for (int i = firstItem; i <= lastItem; i++)
    {
        GroupItem* item = items.at(i);
        nHeight += item->rcItem.height();
    }

    if (nHeight >= groupHeight)
        return;

    int nItems = lastItem - firstItem + 1;
    int nOffset = (groupHeight - nHeight) / (nItems + 1);

    for (int i = firstItem; i <= lastItem; i++)
    {
        GroupItem* item = items.at(i);

        QRect rcControl = item->rcItem;
        rcControl.translate(0, nOffset);
        item->rcItem = rcControl;

        nOffset += nOffset;
    }
}

int GroupLayout::getSizeSpecialDynamicSize_() const
{
    RibbonGroup * rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return 0;

    int totalWidth = 0, x = 0, y = 0, rowHeight = 0;
    int dataCount = items.count();

    int groupHeight = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);
    int height = groupHeight/3;

    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);
        QSize szControl = item->sizeHint();

        height = qMax(height, szControl.height());
        if (item->wrap)
        {
            x = 0;
            y += rowHeight;
        }
        else
        {
            bool beginGroup = item->beginGroup && i > 0;

            if (beginGroup)
                x += 3;
        }
        item->rcItem = QRect(QPoint(2 + x, y), QSize(szControl.width(), height));

        x += szControl.width(); // -1 ???
        rowHeight = qMax(rowHeight, /*szControl.height()*/height);
        totalWidth = qMax(totalWidth, x);
    }
    return totalWidth + 4;
}

bool GroupLayout::findBestWrapSpecialDynamicSize_() const
{
    int nMax = getSizeSpecialDynamicSize_();
    int nMin = 0, nMid = 0;

    int current = wrapSpecialDynamicSize_(nMin, true);
    int target = 2;
    if (current < target)
        return false;

    if (current == target)
        return true;

    while (nMin < nMax)
    {
        nMid = (nMin + nMax) / 2;
        current = wrapSpecialDynamicSize_(nMid, true);

        if (current <= target)
            nMax = nMid;
        else
        {
            if (nMin == nMid)
            {
                wrapSpecialDynamicSize_(nMax, true);
                break;
            }
            nMin = nMid;
        }
    }
    return true;
}

int GroupLayout::wrapSpecialDynamicSize_(int width, bool includeTail) const
{
    int dataCount = items.count();
    int nWraps = 0, x = 0;

    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);

        item->wrap = false;

        QSize szControl = item->sizeHint();//item->rcItem.size();

        bool beginGroup = item->beginGroup && i > 0;
        bool wrapGroup = item->wrapGroup && i > 0;

        if (false)
        {
            x = szControl.width();
            item->wrap = true;
            nWraps++;
            continue;
        }
        else if (wrapGroup)
        {
            if ((includeTail && (x + item->tailWidth >= width)) || 
                (!includeTail && nWraps == 0 && (x + item->tailWidth >= width)))
            {
                x = szControl.width();
                item->wrap = true;
                nWraps++;
                continue;
            }
        }

        if (beginGroup)
            x += 3;

        x += szControl.width();
    }
    return nWraps;
}

void GroupLayout::updateGeomArray_() const
{
    RibbonGroup * rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return;

    int resultHeight = rg->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, rg);

    int dataCount = items.count();

    int tailWidth = 0;
    for (int i = dataCount - 1; i >= 0; i--)
    {
        GroupItem * item = items.at(i);

        QSize szControl = item->sizeHint();

        tailWidth += szControl.width();
        item->tailWidth = tailWidth;

        if (item->wrapGroup)
            tailWidth = 0;
    }

    int baseHeight = resultHeight/3;
    int rowCount = wrapSpecialDynamicSize_(32000, false) + 1;
    int totalWidth = 0;

    if (rowCount == 1)
    {
        rowCount = 3;
        totalWidth = getSizeSpecialDynamicSize_();

        bool allow3Row = resultHeight >= baseHeight * 3;
        bool allow2Row = resultHeight >= baseHeight * 2;

        if (!allow2Row)
        {
            wrapSpecialDynamicSize_(32000, false);
            rowCount = 1;

        }
        else if (!(allow3Row && rowCount_ == 3 && findBestWrapSpecialDynamicSize_()))
        {
            rowCount = wrapSpecialDynamicSize_(totalWidth / 2, false) + 1;
        }
    }

    totalWidth = getSizeSpecialDynamicSize_();

    int nOffset = (resultHeight + 1 - baseHeight * rowCount) / (rowCount + 1);
    int totalOffset = nOffset;
    int nRow = 0;

    const int margin = this->margin();

    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);

        if (i > 0 && item->wrap)
        {
            nOffset = (resultHeight - baseHeight * rowCount - totalOffset) / (rowCount - nRow);
            totalOffset += nOffset;
            nRow++;
        }
        QRect rect(item->rcItem);
        rect.translate(0, totalOffset);
        item->rcItem = rect;
    }

    width_ = totalWidth;

    afterCalcSize();

    GroupLayout* that = const_cast<GroupLayout*>(this);

    QSize sizeResult(totalWidth + 3*margin, resultHeight);
    that->minSize = sizeResult;

    updateBorders(qMax(width_, getMinimumWidth()), QRect());

    that->hint = QSize(sizeResult.width(), sizeResult.height());
    that->dirty = false;
}

bool GroupLayout::extendSize(int widthAvail)
{
    int dataCount = items.count();
    for (int i = dataCount - 1; i >= 0; i--)
    {
        GroupItem * item = items.at(i);
        if (RibbonGallery* widgetGallery = qobject_cast<RibbonGallery*>(item->widget()))
        {
            if (widgetGallery->isResizable())
            {
                RibbonGalleryGroup* items = widgetGallery->galleryGroup();
                int cx = items ? items->getSize().width() : 0;
                if (cx > 0 && cx <= widthAvail)
                {
                    int count = items->getItemCount();
                    QRect rcBorders = widgetGallery->getBorders();
                    int maxWidth = count * cx + rcBorders.left() + rcBorders.right();
                    if (item->szItem.width() + cx <= maxWidth)
                    {
                        item->szItem.setWidth(qMin(maxWidth, item->szItem.width() + int(widthAvail / cx) * cx));
                        return true;
                    }
                }
                return false;
            }
        }
    }
    return false;
}

void GroupLayout::resetLayout()
{
    for (int i = 0; i < items.count(); i++)
    {
        GroupItem* item = items.at(i);
        item->szItem = QSize();
        item->rcItem = QRect();
    }
    updateGeomArray();
}

void GroupLayout::afterCalcSize() const
{
    int dataCount = items.count();
    for (int i = 0; i < dataCount; i++)
    {
        GroupItem * item = items.at(i);
        if (i == 0)
            item->wrap = true;
        item->widget()->setProperty(qtn_Prop_Wrap, item->wrap);
    }
}

QSize GroupLayout::minimumSize() const
{
    if (dirty)
        updateGeomArray();

    return minSize;
}

QSize GroupLayout::sizeHint() const
{
    if ( dirty )
        updateGeomArray();

    return hint;
}

GroupItem* GroupLayout::createItem(QAction* action)
{
    bool customWidget = false;
    bool standardButtonWidget = false;

    QWidget* widget = Q_NULL;
    RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget());
    if (!rg)
        return (GroupItem*)Q_NULL;

    if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action)) 
    {
        widget = widgetAction->requestWidget(rg);
        if (!qobject_cast<QToolButton*>(widget))
        {
            if (widget != Q_NULL) 
            {
                widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
                if (!qobject_cast<RibbonGallery*>(widget))
                    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                customWidget = true;
            }

            if (qobject_cast<QAbstractButton*>(widget))
                QObject::connect(widget, SIGNAL(released()), rg, SIGNAL(released()));
            else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget))
            {
                QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), rg, SLOT(currentIndexChanged(int)));
            }
            else if (qobject_cast<ExWidgetWrapper*>(widget))
            {
                QList<QComboBox*> comboBoxs = qFindChildren<QComboBox*>(widget);
                foreach(QComboBox* i, comboBoxs)
                    QObject::connect(i, SIGNAL(currentIndexChanged(int)), rg, SLOT(currentIndexChanged(int)));
            }
            else if (RibbonGallery* gallery = qobject_cast<RibbonGallery*>(widget))
            {
                QObject::connect(widget, SIGNAL(itemSelectionChanged()), rg, SIGNAL(released()));
                if (OfficePopupMenu* menu = gallery->getPopupMenu())
                {
                    QList<RibbonGallery*> subGallerys = qFindChildren<RibbonGallery*>(menu);
                    foreach(RibbonGallery* i, subGallerys)
                        QObject::connect(i, SIGNAL(itemSelectionChanged()), rg, SIGNAL(released()));
                }
            }
        }
        else
        {
            widget = insertToolButton(rg, action, (QToolButton*)widget);
            standardButtonWidget = true;
        }
    } 
    else if (action->isSeparator()) 
    {
        RibbonSeparator* sep = new RibbonSeparator(rg);
        hasSeparator = true;
        widget = sep;
    }

    if (!widget) 
    {
        QToolButton* button = new RibbonButton(rg);
        widget = insertToolButton(rg, action, button);
        standardButtonWidget = true;
    }

    widget->hide();

    bool beginGroup = false;
    if (hasSeparator && !action->isSeparator()) 
    {
        hasSeparator = false;
        beginGroup = true;
    }

    GroupItem* result = new GroupItem(widget, beginGroup, rg->isControlsGrouping());
    if (standardButtonWidget)
        result->setAlignment(Qt::AlignJustify);

    result->customWidget = customWidget;
    result->action = action;
    return result;
}

QWidget* GroupLayout::insertToolButton(RibbonGroup* rg, QAction* action, QToolButton* button)
{
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QSize szIcon(32, 32);
    if (!action->icon().isNull() && !action->icon().availableSizes().isEmpty() )
        szIcon = action->icon().availableSizes().first();

    button->setIconSize(szIcon);
    action->setIconText(action->text());
    button->setDefaultAction(action);
    QObject::connect(button, SIGNAL(triggered(QAction*)), rg, SIGNAL(actionTriggered(QAction*)));

    if (button->popupMode() != QToolButton::MenuButtonPopup)
        QObject::connect(button, SIGNAL(released()), rg, SIGNAL(released()));

    if (OfficePopupMenu* menu = qobject_cast<OfficePopupMenu*>(action->menu()))
    {
        QList<RibbonGallery*> subGallerys = qFindChildren<RibbonGallery*>(menu);
        foreach(RibbonGallery* i, subGallerys)
            QObject::connect(i, SIGNAL(itemSelectionChanged()), rg, SIGNAL(released()));
    }

    return button;
}

/*!
\class Qtitan::RibbonGroupPrivate
\internal
*/
RibbonGroupPrivate::RibbonGroupPrivate()
//    : layout_(Q_NULL)
    : butOption_(Q_NULL)
    , actOption_(Q_NULL)
    , controlsGrouping_(false)
    , controlsCentering_(false)
{
}

void RibbonGroupPrivate::init()
{
    QTN_P(RibbonGroup);
    p.setMouseTracking(true);

    QLayout* layout = new GroupLayout();
    p.setLayout(layout);

    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout))
        gLayout->updateMarginAndSpacing();

    butOption_ = new RibbonGroupOption(&p);
    actOption_ = new QAction(&p);
    butOption_->setDefaultAction(actOption_);
    QObject::connect(butOption_, SIGNAL(triggered(QAction*)), &p, SIGNAL(actionTriggered(QAction*)));
    butOption_->hide();
}

void RibbonGroupPrivate::layoutWidgets()
{
    QTN_P(RibbonGroup);

    QStyleOptionGroupBox opt;
    p.initStyleOption(opt);

    QRect rc = p.geometry();
    QSize szBut = butOption_->sizeHint();
    QSize sz(qMax(opt.lineWidth - 3, szBut.width()), qMax(opt.lineWidth - 3, szBut.height()));
    QRect rectButOption(QPoint(rc.right() - opt.lineWidth - 1, rc.bottom() - opt.lineWidth), sz);
    butOption_->setGeometry(rectButOption);
}



/* RibbonGroup */
RibbonGroup::RibbonGroup(QWidget* parent, const QString& title)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonGroup);
    QTN_D(RibbonGroup);

    d.init();
    setTitle(title);
}

RibbonGroup::~RibbonGroup()
{
    QTN_FINI_PRIVATE();
}

bool RibbonGroup::isControlsGrouping() const
{
    QTN_D(const RibbonGroup);
    return d.controlsGrouping_;
}

void RibbonGroup::setControlsGrouping(bool controlsGrouping)
{
    QTN_D(RibbonGroup);
    d.controlsGrouping_ = controlsGrouping;
}

void RibbonGroup::setControlsCentering(bool controlsCentering)
{
    QTN_D(RibbonGroup);
    d.controlsCentering_ = controlsCentering;
}

bool RibbonGroup::isControlsCentering() const
{
    QTN_D(const RibbonGroup);
    return d.controlsCentering_;
}

void RibbonGroup::setTitle(const QString& title)
{
    QTN_D(RibbonGroup);
    d.title_ = title;
    adjustSize();
    if (RibbonGroupWrapper* groupWrapper = qobject_cast<RibbonGroupWrapper*>(parentWidget()))
    {
        groupWrapper->setText(title);
        adjustSize();
    }
}

const QString& RibbonGroup::title() const
{
    QTN_D(const RibbonGroup);
    return d.title_;
}

void RibbonGroup::setOptionButtonVisible(bool visible)
{
    QTN_D(const RibbonGroup);
    d.butOption_->setVisible(visible);
    adjustSize();
}

bool RibbonGroup::isOptionButtonVisible() const
{
    QTN_D(const RibbonGroup);
    return d.butOption_->isVisible();
}

QAction* RibbonGroup::getOptionButtonAction() const
{
    QTN_D(const RibbonGroup);
    return d.actOption_;
}

QAction* RibbonGroup::addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, 
                                QMenu* menu, QToolButton::ToolButtonPopupMode mode)
{
    bool setMenuButton = false;
    QAction* action = Q_NULL;
    if (mode == QToolButton::InstantPopup && menu)
    {
        action = menu->menuAction();
        action->setIcon(icon);
        menu->setTitle(text);
    }
    else
    {
        action = new QAction(icon, text, this);
        setMenuButton = true;
    }
    addAction(action);

    QLayoutItem* item = layout()->itemAt(layout()->count()-1);
    if (QToolButton* widgetAction = qobject_cast<QToolButton*>(item ? item->widget() : Q_NULL)) 
    {
        widgetAction->setToolButtonStyle(style);
        if (menu)
        {
            widgetAction->setPopupMode(mode);
            if (setMenuButton)
                widgetAction->setMenu(menu);
        }
    }
    return action;
}

QAction* RibbonGroup::addAction(QAction* action, Qt::ToolButtonStyle style, QMenu* menu, QToolButton::ToolButtonPopupMode mode)
{
    addAction(action);
    QLayoutItem* item = layout()->itemAt(layout()->count()-1);
    if (QToolButton* widgetAction = qobject_cast<QToolButton*>(item ? item->widget() : Q_NULL)) 
    {
        widgetAction->setToolButtonStyle(style);
        if (menu)
        {
            widgetAction->setPopupMode(mode);
            widgetAction->setMenu(menu);
        }
    }
    return action;
}

QAction* RibbonGroup::addWidget(QWidget* widget)
{
    QWidgetAction* action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    widget->setAttribute(Qt::WA_Hover);
    addAction(action);
    return action;
}

QAction* RibbonGroup::addWidget(const QIcon& icon, const QString& text, QWidget* widget)
{
    QWidgetAction* action = new QWidgetAction(this);
    action->setIcon(icon);
    action->setText(text);

    if (qobject_cast<QLineEdit*>(widget) || 
        qobject_cast<QComboBox*>(widget) || 
        qobject_cast<QAbstractSpinBox*>(widget) ||
        qobject_cast<QAbstractSlider*>(widget))
    {
        ExWidgetWrapper* exWidget = new ExWidgetWrapper(this, widget);
        exWidget->setIcon(icon);
        exWidget->setLabelText(text);
        widget = exWidget;
    }

    action->setDefaultWidget(widget);
    widget->setAttribute(Qt::WA_Hover);
    addAction(action);

    return action;
}

QAction* RibbonGroup::addWidget(const QIcon& icon, const QString& text, bool align, QWidget* widget)
{
    QAction* action = addWidget(icon, text, widget);
    if (ExWidgetWrapper* exWidget = qobject_cast<ExWidgetWrapper*>(widget->parentWidget()))
        exWidget->setAlignWidget(align);
    return action;
}

QMenu* RibbonGroup::addMenu(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style)
{
    QMenu* menu = new QMenu(text, this);

    QAction* action = menu->menuAction();
    action->setIcon(icon);

    addAction(action);

    QLayoutItem* item = layout()->itemAt(layout()->count()-1);
    if (QToolButton* widgetAction = qobject_cast<QToolButton*>(item ? item->widget() : Q_NULL)) 
    {
        if (Qt::ToolButtonFollowStyle == style)
        {
            const QSize size = icon.availableSizes().first();
            style = size.width() < 32 ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonTextUnderIcon;
        }
        widgetAction->setToolButtonStyle(style);
        widgetAction->setPopupMode(QToolButton::InstantPopup);
    }
    return menu;
}

QAction* RibbonGroup::addSeparator()
{
    QAction*action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

void RibbonGroup::remove(QWidget* widget)
{
    if (!widget)
        return;

    if (ExWidgetWrapper* exWidget = qobject_cast<ExWidgetWrapper*>(widget->parentWidget()))
        widget = exWidget;

    QList<QAction *> actions = this->actions();
    for(int i = 0; i < actions.size(); i++)
    {
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(actions.at(i)))
        {
            if (widgetAction->defaultWidget()== widget)
            {
                removeAction(actions.at(i));
                return;
            }
        }
    }
}

void RibbonGroup::clear()
{
    QList<QAction*> actions = this->actions();
    for(int i = 0; i < actions.size(); i++)
        removeAction(actions.at(i));
}

QSize RibbonGroup::sizeHint() const
{
    QTN_D(const RibbonGroup);
    QSize sz(layout()->sizeHint());
    QStyleOptionGroupBox opt;
    initStyleOption(opt);
    const int heightGroup = qMax(style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, this), sz.height());
    const int heightCaptionGroup = opt.lineWidth;

    QFontMetrics fm = fontMetrics();
    QSize textSize = fm.size(Qt::TextShowMnemonic, title());
    textSize.setWidth(textSize.width() + fm.width(QLatin1Char(' '))*2);

    int width = qMax(textSize.width() + ADDITIVE_WIDTH, sz.width());
    if (d.butOption_->isVisible())
        width = qMax(width, d.butOption_->width() + textSize.width()/* + 20*/);

    sz.setWidth(width);
    sz.setHeight(heightGroup + heightCaptionGroup + layout()->margin()*2);
    return sz.expandedTo(QApplication::globalStrut());
}

void RibbonGroup::initStyleOption(QStyleOptionGroupBox& opt) const
{
    QTN_D(const RibbonGroup);
    opt.init(this);
    opt.text = d.title_;
    opt.lineWidth = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, this);
    opt.textAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
}

void RibbonGroup::currentIndexChanged(int index)
{
    Q_UNUSED(index);

    if (isVisible())
    {
        bool doReleased = true;
        QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
        if (qobject_cast<ExWidgetWrapper*>(focusWidget()))
        {
            QList<QComboBox*> comboBoxs = qFindChildren<QComboBox*>(sender());
            if (comboBoxs.size() > 0)
                comboBox = comboBoxs.at(0);
        }

        if (comboBox)
        {
            QAbstractItemView* itemView = comboBox->view();
            if (itemView && !itemView->isVisible())
                doReleased = false;
        }
        if (doReleased)
            emit released();
    }
}

bool RibbonGroup::extendSize(int widthAvail)
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        return gLayout->extendSize(widthAvail);
    return false;
}

void RibbonGroup::resetLayout()
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        gLayout->resetLayout();
}

void RibbonGroup::preparationReduced(bool reduced)
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        gLayout->rowCount_ = reduced ? 3 : 2;
    layout()->update();
    layout()->activate();
}

bool RibbonGroup::isMinimizedGroup() const
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        return gLayout->rowCount_ == 3;
    return false;
}

bool RibbonGroup::event(QEvent* event)
{
    if (RibbonGroupWrapper* wrapper = qobject_cast<RibbonGroupWrapper*>(parentWidget()))
    {
        if (!wrapper->isReducedGroup())
        {
            switch(event->type())
            {
            case QEvent::KeyPress :
                {
                    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
                    if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
                    {
                        if (QWidget* focus = focusWidget())
                        {
                            if (qobject_cast<ExWidgetWrapper*>(focus) ||
                                qobject_cast<QComboBox*>(focus)       ||
                                qobject_cast<QLineEdit*>(focus))
                                emit released();
                        }
                    }
                }
                break;
            case QEvent::Show :
                adjustSize();
                break;
            case QEvent::ShowToParent :
                wrapper->show();
                break;
            case QEvent::HideToParent :
                wrapper->hide();
                break;
            default:
                break;
            }
        }
    }
    return QWidget::event(event);
}

void RibbonGroup::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonGroup);

    QPainter p(this);
    QStyleOptionGroupBox opt;
    initStyleOption(opt);

    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_Group, &opt, &p, this);

    int flags = opt.textAlignment;
    flags |= Qt::TextHideMnemonic;

    QRect rcEntryCaption = opt.rect;
    rcEntryCaption.setTop(rcEntryCaption.bottom() - opt.lineWidth);

    int indent = 0;
    if (d.butOption_->isVisible())
        indent = d.butOption_->width();

    rcEntryCaption.adjust(0, -1, -indent, -2);
    style()->drawItemText(&p, rcEntryCaption, flags, opt.palette, opt.state & QStyle::State_Enabled, opt.text, QPalette::WindowText);
}

void RibbonGroup::actionEvent(QActionEvent* event)
{
    QAction* action = event->action();
    QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);

    switch (event->type()) 
    {
        case QEvent::ActionAdded: 
            if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
            {
                Q_ASSERT_X(widgetAction == 0 || gLayout->indexOf(widgetAction) == -1,
                           "RibbonGroup", "widgets cannot be inserted multiple times");

                // reparent the action to this RibbonGroup if it has been created
                // using the addAction(title_) etc. convenience functions, to
                // preserve Qt 4.1.x behavior. The widget is already
                // reparented to us due to the createWidget call inside
                // createItem()
                // if (widgetAction != 0 && widgetAction->d_func()->autoCreated)
                if (widgetAction != Q_NULL)
                    widgetAction->setParent(this);

                int index = layout()->count();
                if (event->before()) 
                {
                    index = gLayout->indexOf(event->before());
                    Q_ASSERT_X(index != -1, "RibbonGroup::insertAction", "internal error");
                }

                if(!qobject_cast<ReducedGroupPopupMenu*>(action->menu()))
                    gLayout->insertAction(index, action);

                break;
            }

        case QEvent::ActionChanged:
            {
                layout()->invalidate();
                QApplication::postEvent(parentWidget()->parentWidget(), new QEvent(QEvent::LayoutRequest));
                break;
            }

        case QEvent::ActionRemoved: 
            if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
            {
                event->action()->disconnect(this);

                int index = gLayout->indexOf(action);
                if (index != -1) 
                {
                    // to remove the shell, if it exists
                    if (QWidgetItem* itemWidget = dynamic_cast<QWidgetItem*>(gLayout->itemAt(index)))
                    {
                        if (ExWidgetWrapper* exWidget = qobject_cast<ExWidgetWrapper*>(itemWidget->widget()))
                        {
                            const QObjectList& listChildren = exWidget->children();
                            for (int i = 0, count = listChildren.count(); count > i; i++)
                            {
                                if (QWidget* widget = qobject_cast<QWidget*>(listChildren[i]))
                                    widget->setParent(Q_NULL);
                            }
                            delete exWidget;
                        }
                    }
                    delete gLayout->takeAt(index);
                }
                QApplication::postEvent(parentWidget()->parentWidget(), new QEvent(QEvent::LayoutRequest));
                break;
            }
        default:
            Q_ASSERT_X(false, "RibbonGroup::actionEvent", "internal error");
    }
}

void RibbonGroup::changeEvent(QEvent* event)
{
    switch (event->type()) 
    {
        case QEvent::FontChange: 
            if (RibbonGroupWrapper* wrapper = qobject_cast<RibbonGroupWrapper*>(parentWidget()))
            {
                if (!wrapper->isReducedGroup())
                {
                    if (QWidget* parent = wrapper->parentWidget()->parentWidget())
                    {
                        QEvent ev(QEvent::LayoutRequest);
                        QApplication::sendEvent(parent, &ev);
                        parent->updateGeometry();
                    }
                }
            }
            break;
        default:
            break;
    };
    return QWidget::changeEvent(event);
}

void RibbonGroup::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    update();
}

void RibbonGroup::leaveEvent( QEvent* event )
{
    QWidget::leaveEvent(event);
    update();
}

void RibbonGroup::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    QTN_D(RibbonGroup);
    d.layoutWidgets();
}

void RibbonGroup::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
}

QWidget* RibbonGroup::getNextWidget(const QWidget* w) const
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        return gLayout->getNextWidget(w);
    return Q_NULL;
}

QWidget* RibbonGroup::getWidget(int index) const
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        return gLayout->getWidget(index);
    return Q_NULL;
}

int RibbonGroup::getIndexWidget(const QWidget* w) const
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        return gLayout->getIndexWidget(w);
    return -1;
}

int RibbonGroup::getNextIndex(const QWidget* w) const
{
    if (GroupLayout* gLayout = qobject_cast<GroupLayout*>(layout()))
        return gLayout->getNextIndex(w);
    return -1;
}
