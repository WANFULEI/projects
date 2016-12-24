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
#ifndef QTN_RIBBONGALLERYPRIVATE_H
#define QTN_RIBBONGALLERYPRIVATE_H

#include <QRect>
#include <QVector>
#include <QVariant>
#include <QBasicTimer>
#include <QIcon>

#include "QtitanDef.h"

class QPainter;
class QScrollBar;
class QAction;

class QMenu;
class QToolButton;

namespace Qtitan
{
    class RibbonGalleryGroup;
    /* WidgetItemData */
    class WidgetItemData
    {
    public:
        inline WidgetItemData() : role(-1) {}
        inline WidgetItemData(int r, QVariant v) : role(r), value(v) {}
        int role;
        QVariant value;
        inline bool operator==(const WidgetItemData &other) { return role == other.role && value == other.value; }
    };

    class RibbonGalleryItem;
    /* RibbonGalleryItemPrivate */
    class RibbonGalleryItemPrivate: public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonGalleryItem)
    public:
        explicit RibbonGalleryItemPrivate();
    public:
        void init();
    public:
        int m_index;
        QString m_descriptionText;
        RibbonGalleryGroup* m_items;
        bool m_label    : 1;
        bool m_enabled  : 1;

        QVector<WidgetItemData> values;
    };

    class RibbonGalleryGroup;
    /* RibbonGalleryGroupPrivate */
    class RibbonGalleryGroupPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonGalleryGroup)
    public:
        explicit RibbonGalleryGroupPrivate();
        virtual ~RibbonGalleryGroupPrivate();

    public:
        void init();
        void updateIndexes(int start /*= 0*/);
        void itemsChanged();
        void redrawWidget();

        void clear();

     public:
        bool clipItems_;
        QSize sizeItem_;
        QVector<RibbonGalleryItem*> arrItems_;
        QList<QWidget*> viewWidgets_;
    };


    class RibbonGallery;

    struct qtn_galleryitem_rect
    {
        RibbonGalleryItem* item;
        QRect rect;
        bool beginRow;
    };

    class OfficePopupMenu;
    /* RibbonGalleryPrivate */
    class RibbonGalleryPrivate : public QObject
    {
    public:
        Q_OBJECT
        QTN_DECLARE_PUBLIC(RibbonGallery)
    public:
        explicit RibbonGalleryPrivate();
        virtual ~RibbonGalleryPrivate();

    public:
        void init();
        void layoutItems();
        void layoutScrollBar();
        void setScrollBarValue();
        void setScrollBarPolicy(Qt::ScrollBarPolicy policy);
        Qt::ScrollBarPolicy scrollBarPolicy() const;
        void setPopupMenu(OfficePopupMenu* popup);

        void drawItems(QPainter* painter);
        void repaintItems(QRect* pRect = Q_NULL, bool bAnimate = true);

        bool isItemChecked(RibbonGalleryItem* pItem) const;
        bool isScrollButtonEnabled(bool buttonUp);

        int scrollWidth() const;

        void startAnimation(int scrollPos);
        void startAnimate();

    public Q_SLOTS:
        void setScrollPos(int nScrollPos);
        void actionTriggered(int action);
        void pressedScrollUp();
        void pressedScrollDown();

    protected:
        virtual bool event(QEvent* event);
        virtual bool eventFilter(QObject* object, QEvent* event);

    public:
        QScrollBar* scrollBar_; 
        QToolButton* buttonScrollUp_;
        QToolButton* buttonScrollDown_;
        QToolButton* buttonPopup_;
        OfficePopupMenu* menuBar_;
        QVector<qtn_galleryitem_rect> arrRects_;

        bool showBorders_       : 1;
        bool showLabels_        : 1;
        bool hideSelection_     : 1;
        bool keyboardSelected_  : 1;
        bool preview_           : 1;
        bool pressed_           : 1;
        bool animation_         : 1;

        int totalHeight_;
        int scrollPos_;
        int scrollPosTarget_;
        double animationStep_;
        int timerElapse_;

        int selected_;
        RibbonGalleryItem* checkedItem_;
        RibbonGalleryGroup* items_;

        QPoint ptPressed_;
        QBasicTimer scrollTimer_;
    };
}; //namespace Qtitan


#endif // QTN_RIBBONGALLERYPRIVATE_H
