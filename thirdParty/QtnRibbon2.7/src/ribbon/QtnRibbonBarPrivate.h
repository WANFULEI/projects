/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonBarPrivate for Qt)
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
#ifndef QTN_RIBBONBARPRIVATE_H
#define QTN_RIBBONBARPRIVATE_H

#include <QStyle>
#include <QStack>

#include "QtnRibbonBar.h"
#include "QtnRibbonTabBar.h"

#include "QtitanDef.h"


namespace Qtitan
{
    class RibbonTabBar;
    class RibbonTitleButton;
    class OfficeFrameHelper;
    class StyleOptionTitleBar;
    class RibbonSystemButton;
    class MenuMinimizedGroups;
    class RibbonKeyTip;

    /* RibbonBarPrivate */
    class RibbonBarPrivate : public QObject
    {
    public:
        Q_OBJECT
        QTN_DECLARE_PUBLIC(RibbonBar)
    public:
        explicit RibbonBarPrivate();
        virtual ~RibbonBarPrivate();

    public:
        void init(); 
        void layoutRibbon();
        void layoutTitleButton();
        void layoutContextHeaders();
        void layoutCorner();

        void removeAllContextHeaders();
        bool reduceContextHeaders();

        void updateMinimizedModePage(RibbonPage* page);
        void swapVisiblePages(int index);

        QAction* createSystemButton(const QIcon& icon, const QString& text);

        RibbonPage* insertPage(int indTab, int index);
        void insertPage(int indTab, int index, RibbonPage* page);
        void removePage(int index, bool deletePage = true);
        int getIndexPage(RibbonPage* page) const;

        void currentChanged(int index);
        bool validIndex(int index) const { return index >= 0 && index < listPages_.count(); }

        void titleBarOptions(StyleOptionTitleBar& opt) const;
        QStyle::SubControl getSubControl(const QPoint& pos) const;

        RibbonTitleButton* findTitleButton(QStyle::SubControl subControl) const;
        QAction* addTitleButton(QStyle::SubControl subControl, bool add, QRect& rcControl);

        ContextHeader* hitTestContextHeaders(const QPoint& point) const;

        void showKeyTips(QWidget* w);
        bool hideKeyTips();
        void createKeyTips();
        void createGroupKeyTips();
        void createWidgetKeyTips(RibbonGroup* group, QWidget* widget, const QString& prefix, const QRect& rcGroups, bool visible);
        bool createPopupKeyTips();
        void destroyKeyTips();
        void calcPositionKeyTips();
        int getMaxHeightPages() const;

    private Q_SLOTS:
        void processClickedSubControl();

    protected:
        bool pressTipCharEvent(const QKeyEvent* key);
        virtual bool eventFilter(QObject*, QEvent* event);

    public:
        QRect rcPageArea_;
        QRect rcTabBar_;
        int currentIndexPage_;
        RibbonTabBar* tabBar_;
        RibbonQuickAccessBar* quickAccessBar_; 
        RibbonSystemButton* controlSystemButton_;
        OfficeFrameHelper* frameHelper_;
        QRect rcTitle_;
        QRect rcHeader_;
        QRect rcTitleText_;
        QRect rcQuickAccess_;

        bool showTitleAlways_;
        bool quickAccessOnTop_;
        bool ribbonBarVisible_;
        bool minimizationEnabled_;
        bool minimized_;
        bool KeyTipsComplement_;

        QAction* actCloseButton_;
        QAction* actNormalButton_;
        QAction* actMaxButton_;
        QAction* actMinButton_;

        QList<ContextHeader*> listContextHeaders_;
        QList<RibbonKeyTip*> keyTips_;
        QStack<QWidget*> levels_;
        int countKey_;

        QStyle::SubControl hoveredSubControl_;
        QStyle::SubControl activeSubControl_;
        QString windowTitle_;
        bool isActive_;

    protected:
        QList<RibbonPage*> listPages_;
    };
}; //namespace Qtitan


#endif // QTN_RIBBONPRIVATE_H


