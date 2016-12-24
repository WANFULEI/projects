/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced Ribbon for Qt)
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
#include <QDebug>
#include <QToolTip>
#include <QPaintEngine>
#include <QApplication>
#include <QCoreApplication>
#include <QComboBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QTabBar>
#include <QBitmap>
#include <QMdiArea>
#include <QStackedWidget>
#include <qevent.h>
#include <QLibrary>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif /* Q_OS_WIN*/


#include "../src/ribbon/QtnRibbonButton.h"
#include "../src/ribbon/QtnRibbonTabBar.h"
#include "../src/ribbon/QtnRibbonPrivate.h"
#include "../src/ribbon/QtnOfficeFrameHelper.h"
#include "../src/ribbon/QtnRibbonBarPrivate.h"
#include "../src/ribbon/QtnRibbonBackstageView.h"
#include "QtnRibbonStyle.h"
#include "QtnStyleHelpers.h"
#include "QtnCommonStylePrivate.h"
#include "QtitanRibbon.h"
#include "QtnRibbonQuickAccessBar.h"
#include "QtnRibbonGallery.h"

using namespace Qtitan;

static const int windowsSepHeight        =  9; // height of the separator
static const int windowsItemFrame        =  2; // menu item frame width
static const int windowsItemHMargin      =  3; // menu item hor text margin
static const int windowsCheckMarkWidth   = 16; // checkmarks width on windows
static const int windowsItemVMargin      =  2; // menu item ver text margin
static const int windowsRightBorder      = 15; // right border on windows
static const int windowsTabSpacing       = 12; // space between text and tab
static const int windowsArrowHMargin     =  6; // arrow horizontal margin
//static const int logPixel                = 96;


#ifdef Q_OS_WIN
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled = 0;
#endif /* Q_OS_WIN*/

static bool use2000style = true;


template<class T>
static const T* getParentWidget(const QWidget* widget)
{
    while (widget)
    {
        if (const T* parent = qobject_cast<const T*>(widget))
            return parent;
        widget = widget->parentWidget();
    }
    return Q_NULL;
}


namespace Qtitan
{
    /* RibbonPagePrivate */
    class RibbonStylePrivate : QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonStyle)
    public:
        explicit RibbonStylePrivate();

    public:
        void initialization();
        void refreshMetrics(QWidget* widget);
        void updateColors();

        void tabLayout(const QStyleOptionTabV3* opt, const QWidget* widget, QRect* textRect, QRect* iconRect) const;
        void tabHoverUpdate(QTabBar* tabBar, QEvent* event);

        void setMacSmallSize(QWidget* widget);
        void unsetMacSmallSize(QWidget* widget);

    public:
        int baseHeight_;
        int tabsHeight_;
        int rowHeight_;
        int fileButtonImageCount_;
        bool flatFrame_;
        bool destroyKeyTips_;
        bool completeKey_;
        QRect oldHoverRectTab_;
        QFont fontRegular_;  // regular font.
        QHash<const QWidget*, bool> macSmallSizeWidgets_;
        static int logPixel;
    };
}

int RibbonStylePrivate::logPixel = 96;
RibbonStylePrivate::RibbonStylePrivate()
{
    baseHeight_ = -1;
    tabsHeight_ = 22;
    rowHeight_ = 22;
    fileButtonImageCount_ = 3;
    flatFrame_ = false;
    destroyKeyTips_ = false;
    completeKey_ = false;

#ifdef Q_OS_WIN
    QLibrary dwmLib(QString::fromAscii("dwmapi"));
    pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
#endif /* Q_OS_WIN*/
}

void RibbonStylePrivate::initialization()
{
    tabsHeight_ = rowHeight_ > 22 ? rowHeight_ + 2 : 23;
    refreshMetrics(Q_NULL);
    updateColors();
}


#ifdef Q_OS_WIN
QFont qtn_LOGFONTtoQFont(LOGFONT& lf)
{
    bool dwmEnabled = false;
    if (pDwmIsCompositionEnabled) 
    {
        BOOL enabled;
        HRESULT hr = pDwmIsCompositionEnabled(&enabled);
        dwmEnabled = (SUCCEEDED(hr) && enabled);
    }

    QString family = QString::fromWCharArray(lf.lfFaceName);
    QFont qf(family);
    qf.setItalic(lf.lfItalic);
    if (lf.lfWeight != FW_DONTCARE) 
    {
        int weight;
        if (lf.lfWeight < 400)
            weight = QFont::Light;
        else if (lf.lfWeight < 600)
            weight = QFont::Normal;
        else if (lf.lfWeight < 700)
            weight = QFont::DemiBold;
        else if (lf.lfWeight < 800)
            weight = QFont::Bold;
        else
            weight = QFont::Black;
        qf.setWeight(weight);
    }
    int lfh = qAbs(lf.lfHeight);

    HDC displayDC = GetDC(0);

    double scale = 64.0;
    if (QSysInfo::windowsVersion() <= QSysInfo::WV_XP || !dwmEnabled) 
        scale = 72.0;

    qf.setPointSizeF(lfh * scale / GetDeviceCaps(displayDC, LOGPIXELSY));
    ReleaseDC(0, displayDC);

    qf.setUnderline(false);
    qf.setOverline(false);
    qf.setStrikeOut(false);
    return qf;
}
#endif 

void RibbonStylePrivate::refreshMetrics(QWidget* widget)
{
    QTN_P(RibbonStyle);

#ifdef Q_OS_WIN
    NONCLIENTMETRICS ncm;
    ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICS, lfMessageFont) + sizeof(LOGFONT);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize , &ncm, 0);
    baseHeight_ = qAbs(ncm.lfMenuFont.lfHeight);
    fontRegular_ = qtn_LOGFONTtoQFont(ncm.lfMenuFont);

    if (widget)
    {
        QFont font = widget->font();
        if (font != fontRegular_)
        {
            HFONT hf = font.handle();
            LOGFONT lf;
            ::GetObject(hf, sizeof(lf), &lf);
            baseHeight_ = qAbs(lf.lfHeight);
            fontRegular_ = qtn_LOGFONTtoQFont(lf);
        }
    }

    if (baseHeight_ <= 12) 
       baseHeight_ = 11;

    rowHeight_ = qMax(22, baseHeight_ * 195 / logPixel - 3);

    QTabBar tab;
    tab.setFont(qobject_cast<Qtitan::RibbonTabBar*>(widget) ? widget->parentWidget()->font() : fontRegular_);
    tab.insertTab(0, QObject::tr("TEXT"));

    int hframe = qMax(p.proxy()->pixelMetric(QStyle::PM_TabBarTabHSpace, Q_NULL, Q_NULL), tab.sizeHint().height());
    int heightTab = qMax(22, hframe + 1);
    tabsHeight_ = heightTab > 22 ? heightTab : 23;
#else 
    QMenuBar menu;
    menu.setAttribute(Qt::WA_MacSmallSize);
    fontRegular_ = widget ? widget->font() : QApplication::font(&menu);

    QLineEdit ed;
    ed.setAttribute(Qt::WA_MacSmallSize);
    ed.setFont(fontRegular_);
    QSize sz = ed.sizeHint();
    rowHeight_ = qMax(22, sz.height());

    QTabBar tab;
    tab.setFont(qobject_cast<Qtitan::RibbonTabBar*>(widget) ? widget->parentWidget()->font() : fontRegular_);
    tab.setAttribute(Qt::WA_MacSmallSize);
    tab.insertTab(0, QObject::tr("TEXT"));

    int hframe = qMax(p.proxy()->pixelMetric(QStyle::PM_TabBarTabHSpace, Q_NULL, Q_NULL), tab.sizeHint().height());
    int heightTab = qMax(22, hframe + 1);
    tabsHeight_ = heightTab > 22 ? heightTab : 23;
#endif 
}

void RibbonStylePrivate::updateColors()
{
    QTN_P(RibbonStyle);

    p.m_clrFileButtonText = QColor(255, 255, 255);
    p.m_clrRibbonGrayText = p.m_clrMenuBarGrayText;

    StyleHelper& helper = p.helper();

    // [Ribbon]
    p.m_clrRibbonGroupCaptionText = helper.getColor(QObject::tr("Ribbon"), QObject::tr("GroupCaptionText"));
    p.m_clrRibbonFace = helper.getColor(QObject::tr("Ribbon"), QObject::tr("RibbonFace"));
    p.m_clrRibbonText = helper.getColor(QObject::tr("Ribbon"), QObject::tr("RibbonText"));
    p.m_clrRibbonInactiveFace = helper.getColor(QObject::tr("Ribbon"), QObject::tr("RibbonInactiveFace"), p.m_clrRibbonFace);

    p.m_clrMinimizedFrameEdgeHighLight = helper.getColor(QObject::tr("Ribbon"), QObject::tr("MinimizedFrameEdgeHighLight"));
    p.m_clrMinimizedFrameEdgeShadow = helper.getColor(QObject::tr("Ribbon"), QObject::tr("MinimizedFrameEdgeShadow"));
    flatFrame_ = (bool)helper.getInteger(QObject::tr("Window"), QObject::tr("FlatFrame"), 0);
    fileButtonImageCount_ = helper.getInteger(QObject::tr("Ribbon"), QObject::tr("FileButtonImageCount"), 3);
}

void RibbonStylePrivate::tabLayout(const QStyleOptionTabV3* opt, const QWidget* widget, QRect* textRect, QRect* iconRect) const
{
    QTN_P(const RibbonStyle);
    const QStyle* proxyStyle = p.proxy();

    Q_ASSERT(textRect);
    Q_ASSERT(iconRect);
    QRect tr = opt->rect;
    bool verticalTabs = opt->shape == QTabBar::RoundedEast
        || opt->shape == QTabBar::RoundedWest
        || opt->shape == QTabBar::TriangularEast
        || opt->shape == QTabBar::TriangularWest;

    bool botton = opt->shape == QTabBar::RoundedSouth;

    if (verticalTabs)
        tr.setRect(0, 0, tr.height(), tr.width()); //0, 0 as we will have a translate transform

    int verticalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
    int horizontalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);

    int xShift = botton ? 0 : 5;
    int hpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2 - xShift;
    int vpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;

    if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
        verticalShift = -verticalShift;

    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);

    bool selected = opt->state & QStyle::State_Selected;
    if (selected)
    {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty())
    {
        tr.setLeft(tr.left() + 4 +
            (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    }
    // right widget
    if (!opt->rightButtonSize.isEmpty())
    {
        tr.setRight(tr.right() - 4 -
            (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
    }

    // icon
    if (!opt->icon.isNull())
    {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid())
        {
            int iconExtent = proxyStyle->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(iconSize,
            (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
            (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off  );

        *iconRect = QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2,
            tabIconSize.width(), tabIconSize .height());

        xShift = botton ? 6 : 0;

        if (!verticalTabs && botton && !((opt->position == QStyleOptionTab::OnlyOneTab || opt->position == QStyleOptionTab::Beginning)))
            xShift = 0;

        iconRect->translate(xShift, 0);

        if (!verticalTabs)
            *iconRect = proxyStyle->visualRect(opt->direction, opt->rect, *iconRect);

        xShift = botton ? /*2 +*/ tabIconSize.width() : 4;

        if (!verticalTabs && botton && !((opt->position == QStyleOptionTab::OnlyOneTab || opt->position == QStyleOptionTab::Beginning)))
            xShift -= 7;

        tr.translate(xShift, 0);
    }

    if (!verticalTabs)
        tr = proxyStyle->visualRect(opt->direction, opt->rect, tr);
    
    *textRect = tr;
}

void RibbonStylePrivate::tabHoverUpdate(QTabBar* tabBar, QEvent* event)
{
    if (event->type() == QEvent::HoverMove || event->type() == QEvent::HoverEnter)
    {
        QHoverEvent* he = static_cast<QHoverEvent*>(event);
        int index = tabBar->tabAt(he->pos());
        if (index != -1)
        {
            QRect rect = tabBar->tabRect(index);
            switch (tabBar->shape()) 
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                    rect.adjust(0, 0, 11, 0);
                    break;
                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
//                    if (tab->position != QStyleOptionTab::Beginning)
                        rect.adjust(-11, 0, 0, 0);
                    break;
                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                    rect.adjust(0, -11, 0, 0);
                    break;
                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                    rect.adjust(0, -11, 0, 0);
                    break;
                default:
                    break;
            }

            oldHoverRectTab_ = rect;
            tabBar->update(rect);
        }
    } 
    else if (event->type() == QEvent::HoverLeave)
    {
        tabBar->update(oldHoverRectTab_);
        oldHoverRectTab_ = QRect();
    }
}

void RibbonStylePrivate::setMacSmallSize(QWidget* widget)
{
    macSmallSizeWidgets_.insert(widget, widget->testAttribute(Qt::WA_MacSmallSize));
    widget->setAttribute(Qt::WA_MacSmallSize, true);
}

void RibbonStylePrivate::unsetMacSmallSize(QWidget* widget)
{
    if (macSmallSizeWidgets_.contains(widget)) 
    {
        bool testMacSmallSize = macSmallSizeWidgets_.value(widget);
        widget->setAttribute(Qt::WA_MacSmallSize, testMacSmallSize);
        macSmallSizeWidgets_.remove(widget);
    }
}



/* RibbonStyle */
RibbonStyle::RibbonStyle()
    : OfficeStyle()
{
    QTN_INIT_PRIVATE(RibbonStyle);
    QTN_D(RibbonStyle);
    d.initialization();
}

RibbonStyle::RibbonStyle(QMainWindow* mainWindow)
    : OfficeStyle(mainWindow)
{
    QTN_INIT_PRIVATE(RibbonStyle);
    QTN_D(RibbonStyle);
    d.initialization();
}

RibbonStyle::~RibbonStyle()
{
    QTN_FINI_PRIVATE();
}

static void resetPolished(QWidget* w) 
{
    w->setAttribute(Qt::WA_WState_Polished, true);
    w->setFont(QFont());

    QList<QWidget*> widgets = qFindChildren<QWidget*>(w);
    foreach (QWidget* wd, widgets)
        resetPolished(wd);
}

void RibbonStyle::polish(QApplication* application)
{
    OfficeStyle::polish(application);

    if (application)
        application->installEventFilter(this);

    QTN_D(RibbonStyle);
    d.refreshMetrics(Q_NULL);
    d.updateColors();
}

void RibbonStyle::unpolish(QApplication* application)
{
    OfficeStyle::unpolish(application);

    if (application)
        application->removeEventFilter(this);
}

void RibbonStyle::polish(QWidget* widget)
{
    OfficeStyle::polish(widget);
    QTN_D(RibbonStyle);

/*
    if ((getParentWidget<RibbonBar>(widget) && !qobject_cast<QMenu*>(widget)) || 
        qobject_cast<RibbonBar*>(widget)   ||
        qobject_cast<ReducedGroupPopupMenu*>(widget))
    {
        d.setMacSmallSize(widget);
        widget->setFont(d.fontRegular_);
    }
*/
    if (qobject_cast<RibbonBar*>(widget))
    {
        d.setMacSmallSize(widget);
        widget->setFont(d.fontRegular_);

        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, m_clrRibbonText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<RibbonGroup*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, m_clrMenuBarGrayText);
        palette.setColor(QPalette::WindowText, m_clrRibbonGroupCaptionText);
        palette.setColor(QPalette::ButtonText, m_clrMenuPopupText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<RibbonGroupWrapper*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, m_clrToolBarGrayText);
        palette.setColor(QPalette::WindowText, m_clrToolBarText);
        widget->setPalette(palette);
    }
    else if ((qobject_cast<RibbonGroup*>(widget->parentWidget()) &&
             qobject_cast<QToolButton*>(widget)))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, m_clrToolBarGrayText);
        palette.setColor(QPalette::WindowText, m_clrToolBarText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<RibbonSystemPopupBar*>(widget->parentWidget()) && qobject_cast<QToolButton*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, m_clrMenuPopupText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<RibbonSystemButton*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::ButtonText, m_clrFileButtonText);
        widget->setPalette(palette);
    }
/*
    else if (getParentWidget<RibbonBar>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, m_clrRibbonText);
        widget->setPalette(palette);
    }
*/
    else if (qobject_cast<OfficePopupMenu*>(widget->parentWidget()) && qobject_cast<QLabel*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::WindowText, getTextColor(false, false, true, false, false, TypePopup, BarNone));
        widget->setPalette(palette);
    }
    else if (qobject_cast<QMdiArea*>(widget))
    {
        QPalette palette = widget->palette();
        QColor color = helper().getColor(tr("TabManager"), tr("AccessBorder"));
        QColor light = color.lighter(260);
        QColor dark = color.darker(120);
        dark.setAlpha(155);
        palette.setColor(QPalette::Dark, dark);
        palette.setColor(QPalette::Light, light);
        widget->setPalette(palette);
    }
    else if (qobject_cast<QTabBar*>(widget) && qobject_cast<QMdiArea*>(widget->parentWidget()))
    {
        widget->setAutoFillBackground(true);

        QPalette palette = widget->palette();
        palette.setColor(QPalette::Background, helper().getColor(tr("TabManager"), tr("AccessHeader")));
        widget->setPalette(palette);

        ((QTabBar*)widget)->setExpanding(false);
        widget->installEventFilter(this);
    }
}

void RibbonStyle::unpolish(QWidget* widget)
{
    OfficeStyle::unpolish(widget);
    QTN_D(RibbonStyle);

    if (qobject_cast<RibbonBar*>(widget))
        resetPolished(widget); 

    if (getParentWidget<RibbonBar>(widget) || qobject_cast<RibbonBar*>(widget)) 
        d.unsetMacSmallSize(widget);

    if (static_cast<RibbonBar*>((RibbonBar*)widget))
        widget->removeEventFilter(this);

    if (qobject_cast<QTabBar*>(widget) && qobject_cast<QMdiArea*>(widget->parentWidget()))
    {
        widget->removeEventFilter(this);
        ((QTabBar*)widget)->setExpanding(true);
    }
}

void RibbonStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    int qtnElement = element;

    switch (qtnElement)
    {
        case PE_RibbonPopupBarButton :
                drawPopupSystemButton(option, painter, widget);
            break;
        case PE_RibbonFileButton :
                drawSystemButton(option, painter, widget);
            break;
        case PE_RibbonOptionButton :
                drawOptionButton(option, painter, widget);
            break;
        case PE_RibbonQuickAccessButton :
                drawQuickAccessButton(option, painter, widget);
            break;
        case PE_RibbonTab :
                drawTabShape(option, painter, widget);
            break;
        case PE_RibbonContextHeaders:
                drawContextHeaders(option, painter);
            break;
        case PE_RibbonFillRect :
                drawFillRect(option, painter, widget);
            break;
        case PE_RibbonRect :
                drawRect(option, painter, widget);
            break;
        case PE_RibbonKeyTip :
                drawKeyTip(option, painter, widget);
            break;
        case PE_Backstage :
                drawBackstage(option, painter, widget);
            break;
        case PE_RibbonSliderButton :
            {
                QPixmap soButton = option->direction == Qt::RightToLeft ? cached("SliderUp.png") : cached("SliderDown.png");
//                bool enabled  = option->state & State_Enabled;
                bool selected = option->state & State_MouseOver;
                bool pressed  = option->state & State_Sunken;
                int state = 0;
                if (pressed)
                    state = 2;
                else if (selected)
                    state = 1;
                drawImage(soButton, *painter, option->rect, getSource(soButton.rect(), state, 3));
                break;
            }
        default:
            OfficeStyle::drawPrimitive(element, option, painter, widget);
    }
}

void RibbonStyle::drawControl(ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    switch (element)
    {
        case CE_RibbonTabShapeLabel:
                drawTabShapeLabel(opt, p, widget);
            break;
        case CE_RibbonBar : 
                drawRibbonBar(opt, p, widget);
             break;
        case CE_RibbonFileButtonLabel :
                drawFileButtonLabel(opt, p, widget);
            break;
        case CE_RadioButtonLabel:
        case CE_CheckBoxLabel: 
            {
                if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(opt))
                {
                    uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);
                    if (!proxy()->styleHint(SH_UnderlineShortcut, btn, widget))
                        alignment |= Qt::TextHideMnemonic;
                    QPixmap pix;
                    QRect textRect = btn->rect;
                    if (!btn->icon.isNull()) 
                    {
                        pix = btn->icon.pixmap(btn->iconSize, btn->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
                        proxy()->drawItemPixmap(p, btn->rect, alignment, pix);
                        if (btn->direction == Qt::RightToLeft)
                            textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
                        else
                            textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
                    }
                    if (!btn->text.isEmpty())
                        proxy()->drawItemText(p, textRect, alignment | Qt::TextShowMnemonic, btn->palette, 
                            btn->state & State_Enabled, btn->text, QPalette::ButtonText);
                }
                break;
            }
        case CE_RibbonGroups: 
                drawRibbonGroups(opt, p, widget);
            break;
        case CE_Group : 
                drawGroup(opt, p, widget);
            break;
        case CE_ReducedGroup : 
                drawReducedGroup(opt, p, widget);
            break;
        case CE_PopupSizeGrip :
                drawPopupResizeGripper(opt, p, widget);
            break;
        default:
                OfficeStyle::drawControl(element, opt, p, widget);
            break;
    }
}

void RibbonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    OfficeStyle::drawComplexControl(cc, option, painter, widget);
}

int RibbonStyle::pixelMetric(PixelMetric pm, const QStyleOption* option, const QWidget* widget) const
{
    int ret = 0;
    QTN_D(const RibbonStyle);
    switch(pm) 
    {
        case PM_MenuBarVMargin:
            {
                ret = OfficeStyle::pixelMetric(pm, option, widget);
                if (!option)
                {
                    if (const RibbonBar* ribbonBar = qobject_cast<const RibbonBar*>(widget))
                        ret += ribbonBar->topBorder() + ribbonBar->titleBarHeight() + 2;
                }
            }
            break;
        case PM_TabBarTabHSpace :
            {
                ret = OfficeStyle::pixelMetric(pm, option, widget);
                if (widget && qobject_cast<QMdiArea*>(widget->parentWidget()))
                {
                    if (const QStyleOptionTab* taOption = qstyleoption_cast<const QStyleOptionTab*>(option))
                    {
                        bool verticalTabs = taOption->shape == QTabBar::RoundedEast
                            || taOption->shape == QTabBar::RoundedWest
                            || taOption->shape == QTabBar::TriangularEast
                            || taOption->shape == QTabBar::TriangularWest;

                        bool botton = taOption->shape == QTabBar::RoundedSouth;
                        if (!verticalTabs && botton)
                        {
                            if (taOption->icon.isNull() && (taOption->position == QStyleOptionTab::OnlyOneTab || taOption->position == QStyleOptionTab::Beginning))
                                ret += 11;
                            else if (!taOption->icon.isNull() && !(taOption->position == QStyleOptionTab::OnlyOneTab || taOption->position == QStyleOptionTab::Beginning))
                                ret -= 11;
                        }
                    }
                }
            }
            break;
        case PM_RibbonReducedGroupWidth : 
            if(const QStyleOptionToolButton* but = qstyleoption_cast<const QStyleOptionToolButton*>(option))
            {
                QString strFirstRow, strSecondRow;
                CommonStyle::splitString(but->text, strFirstRow, strSecondRow);
                if(strFirstRow.count() < strSecondRow.count())
                    strFirstRow = strSecondRow;
                    
                QFontMetrics fm(but->fontMetrics);
                int wid = fm.width(strFirstRow)+6;
                    
                QPixmap soCenter = cached("RibbonGroupButton.png");
                QRect rcSrc = getSource(soCenter.rect(), 0, 3);
                ret = qMax(ret, qMax(wid, rcSrc.width()));
                break;
            }
        case PM_RibbonHeightGroup :
            {
                ret = d.rowHeight_*3;
                break;
            }
        case PM_RibbonHeightCaptionGroup :
            {
                ret = option->fontMetrics.height() + 3;
                break;
            }
        case PM_RibbonTabsHeight :
            {
                ret = d.tabsHeight_;
                break;
            }
        case PM_TitleBarHeight : 
            {
                ret = OfficeStyle::pixelMetric(pm, option, widget);
/*
                QPixmap soCenter = cached("FrameTopCenter.png");
                ret = qMax(ret, getSource(soCenter.rect(), 0, 2).height());
*/
                break;
            }
        case PM_MenuHMargin :
            {
                if (qobject_cast<const RibbonSystemPopupBar*>(widget)) 
                    ret = 6;
                else if(qobject_cast<const OfficePopupMenu*>(widget))
                    ret = -1;
                else
                    ret = OfficeStyle::pixelMetric( pm, option, widget );
                break;
            }
        case PM_MenuVMargin :
            {
                if (qobject_cast<const RibbonSystemPopupBar*>(widget)) 
                    ret = 19;
                else if(qobject_cast<const RibbonBackstageView*>(widget))
                    ret = 7;
                else if(qobject_cast<const OfficePopupMenu*>(widget))
                    ret = 0;
                else
                    ret = OfficeStyle::pixelMetric(pm, option, widget);
                break;
            }
        case PM_ToolBarIconSize :
            {
                if (qobject_cast<const RibbonSystemPopupBar*>(widget)) 
                    ret = 32;
                else
                    ret = OfficeStyle::pixelMetric( pm, option, widget );
                break;
            }
        case PM_ToolBarFrameWidth :
            {
                if (qobject_cast<const RibbonQuickAccessBar*>(widget)) 
                    ret = 0;
                else
                    ret = OfficeStyle::pixelMetric( pm, option, widget );
                break;
            }
        // Returns the number of pixels to use for the business part of the
        // slider (i.e., the non-tickmark portion). The remaining space is shared
        // equally between the tickmark regions.
        case PM_SliderControlThickness :
            {
                if (const QStyleOptionSlider *sl = qstyleoption_cast<const QStyleOptionSlider *>(option)) 
                {
                    int space = (sl->orientation == Qt::Horizontal) ? sl->rect.height() : sl->rect.width();
                    int ticks = sl->tickPosition;
                    int n = 0;
                    if (ticks & QSlider::TicksAbove)
                        ++n;
                    if (ticks & QSlider::TicksBelow)
                        ++n;
                    if (!n) 
                    {
                        ret = space;
                        break;
                    }

                    int thick = 6;  // Magic constant to get 5 + 16 + 5
                    if (ticks != QSlider::TicksBothSides && ticks != QSlider::NoTicks)
                        thick += proxy()->pixelMetric(PM_SliderLength, sl, widget) / 4;

                    space -= thick;
                    if (space > 0)
                        thick += (space * 2) / (n + 2);
                    ret = thick;
                } else {
                    ret = 0;
                }
           }
            break;

        default:
            ret = OfficeStyle::pixelMetric(pm, option, widget);
        break;
    }
    return ret;
}

int RibbonStyle::styleHint(StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn *returnData) const
{
    QTN_D(const RibbonStyle);
    int ret = 0;
    if (hint == QStyle::SH_ToolButtonStyle)
    {
        if (const RibbonSystemButton* sysButton = qobject_cast<const RibbonSystemButton*>(widget))
            ret = sysButton->toolButtonStyle();
    }
    else if (hint == (StyleHint)SH_FlatFrame)
    {
        ret = d.flatFrame_;
    }
    else if (hint == SH_UnderlineShortcut)
    {
        const RibbonBar* ribbonBar = ::getParentWidget<const RibbonBar>(widget);
        if (ribbonBar && !qobject_cast<const QMenu*>(widget))
            ret = 0;
        else if (ribbonBar && ribbonBar->qtn_d().keyTips_.size() > 0)
            ret = 1;
        else
            ret = OfficeStyle::styleHint(hint, opt, widget, returnData);
    }
    else
        ret = OfficeStyle::styleHint(hint, opt, widget, returnData);
    return ret;
}

QPixmap RibbonStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption* opt, const QWidget* widget) const
{
    return OfficeStyle::standardPixmap( standardPixmap, opt, widget );
}

QSize RibbonStyle::sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& csz, const QWidget* widget) const
{
    QSize sz(csz);
    switch (ct)
    {
        case CT_ToolButton:
            {
                if (qobject_cast<const RibbonQuickAccessBar*>(widget->parentWidget()))
                {
                    sz = csz;
                    sz.setWidth(csz.width()-1);
                    sz.setHeight(csz.height()-2);
                } 
                else if (qobject_cast<const RibbonSystemButton*>(widget))
                {
                    sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
                }
                else
                {
                    sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
                }
            }
            break;
        case CT_MenuItem :
            {
                if (const RibbonSystemPopupBar* menu = qobject_cast<const RibbonSystemPopupBar*>(widget))
                {
                    if (const QStyleOptionMenuItem* mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                    {
                        int w = sz.width();
                        sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);

                        if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                        {
                            sz = QSize(10, windowsSepHeight);
                        }
                        else if (mi->icon.isNull()) 
                        {
                            sz.setHeight(sz.height() - 2);

                            int maxheight = 0;
                            QList<QAction*> list = menu->actions();
                            for (int j = 0; j < list.count(); j++)
                            {
                                QAction* action = list.at(j);
                                if (!action->icon().isNull())
                                {
                                    int iconExtent = pixelMetric(PM_LargeIconSize, opt, widget);
                                    maxheight = qMax(sz.height(), action->icon().actualSize(QSize(iconExtent, iconExtent)).height()+ 2 * windowsItemFrame);
                                }
                            }

                            if (maxheight > 0)
                                sz.setHeight(maxheight);

                            w -= 6;
                        }

                        if (mi->menuItemType != QStyleOptionMenuItem::Separator && !mi->icon.isNull()) 
                        {
                            int iconExtent = pixelMetric(PM_LargeIconSize, opt, widget);
                            sz.setHeight(qMax(sz.height(), mi->icon.actualSize(QSize(iconExtent, iconExtent)).height()+ 2 * windowsItemFrame));
                        }
                        int maxpmw = mi->maxIconWidth;
                        int tabSpacing = use2000style ? 20 :windowsTabSpacing;
                        if (mi->text.contains(QLatin1Char('\t')))
                            w += tabSpacing;
                        else if (mi->menuItemType == QStyleOptionMenuItem::SubMenu)
                            w += 2 * windowsArrowHMargin;
                        else if (mi->menuItemType == QStyleOptionMenuItem::DefaultItem) 
                        {
                            // adjust the font and add the difference in size.
                            // it would be better if the font could be adjusted in the initStyleOption qmenu func!!
                            QFontMetrics fm(mi->font);
                            QFont fontBold = mi->font;
                            fontBold.setBold(true);
                            QFontMetrics fmBold(fontBold);
                            w += fmBold.width(mi->text) - fm.width(mi->text);
                        }

                        int checkcol = qMax(maxpmw, windowsCheckMarkWidth); // Windows always shows a check column
                        w += checkcol;
                        w += windowsRightBorder+12;// + 10;
                        sz.setWidth(w);

                    }
                }
                else if(qobject_cast<const OfficePopupMenu*>(widget))
                {
                    if (const QStyleOptionMenuItem* mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) 
                    {
                        sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
                        if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                            sz.setHeight(csz.height());
                    }
                }
                else
                    sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
            }
            break;
        case CT_RibbonSliderButton :
            {
                QPixmap soButton;
                soButton = opt->direction == Qt::RightToLeft ? cached("SliderUp.png") : cached("SliderDown.png");
                sz = getSource(soButton.rect(), opt->state==QStyle::State_None ? 0 : opt->state&QStyle::State_Sunken? 2 : 1, 3).size();
            }
            break;
        default:
            sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
    }

    return sz;
}

QRect RibbonStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const
{
    QRect ret;
    switch (cc)
    {
        case CC_ToolButton:
            if (const QStyleOptionToolButton* tb = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
            {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, tb, widget);
                ret = tb->rect;
                switch (sc) 
                {
                case SC_ToolButton:
                    if ((tb->features& (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay))== QStyleOptionToolButton::MenuButtonPopup)
                    {
                        if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon) 
                        {
                            QRect popupr = subControlRect(cc, opt, QStyle::SC_ToolButtonMenu, widget);
                            ret.adjust(0, 0, 0, -popupr.height());
                        }
                        else
                            ret.adjust(0, 0, -mbi, 0);
                        break;
                    }
                case SC_ToolButtonMenu:
                    if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay)) == QStyleOptionToolButton::MenuButtonPopup)
                    {
                        if (!qobject_cast<const RibbonGroupWrapper*>(widget)) 
                        {
                            if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon) 
                            {
                                QString str(tb->text);
                                QString strFirstRow, strSecondRow;
                                splitString(str, strFirstRow, strSecondRow);

                                QSize textSize;
                                if (!strFirstRow.isEmpty())
                                {
                                    textSize = opt->fontMetrics.size(Qt::TextShowMnemonic, strFirstRow);
                                    textSize.setWidth(textSize.width() + opt->fontMetrics.width(QLatin1Char(' '))*2);
                                }
                                if (!strSecondRow.isEmpty())
                                {
                                    QSize textSize1 = opt->fontMetrics.size(Qt::TextShowMnemonic, strSecondRow);
                                    textSize1.setWidth(textSize1.width() + opt->fontMetrics.width(QLatin1Char(' '))*2);
                                    textSize.setWidth(qMax(textSize.width(), textSize1.width()));
                                }
                                ret.adjust(0, tb->iconSize.height()+8, 0, 0);
                                ret.setWidth(qMax(textSize.width(), opt->rect.width()));
                            } else
                                ret.adjust(ret.width() - mbi, 0, 0, 0);
                        }
                        break;
                    }
                default:
                    break;
                }
                ret = visualRect(tb->direction, tb->rect, ret);
            }
            break;
        default:
            ret = OfficeStyle::subControlRect(cc, opt, sc, widget);
    }
    return ret;
}

QRect RibbonStyle::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
{
    QRect r;
    switch (sr)
    {
        case SE_TabBarTabText:
            if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(opt))
            {
                if (qobject_cast<QMdiArea*>(widget->parentWidget()))
                {
                    QTN_D(const RibbonStyle);
                    QStyleOptionTabV3 tabV3(*tab);
                    QRect dummyIconRect;
                    d.tabLayout(&tabV3, widget, &r, &dummyIconRect);

                    bool verticalTabs = tab->shape == QTabBar::RoundedEast
                        || tab->shape == QTabBar::RoundedWest
                        || tab->shape == QTabBar::TriangularEast
                        || tab->shape == QTabBar::TriangularWest;

                    bool botton = tab->shape == QTabBar::RoundedSouth;
                    if (!verticalTabs && botton && tab->icon.isNull() && 
                        (tab->position == QStyleOptionTab::OnlyOneTab || tab->position == QStyleOptionTab::Beginning))
                        r.translate(6, 0);
                }
                else
                    r = OfficeStyle::subElementRect(sr, opt, widget);
            }
            break;
        default:
            r = OfficeStyle::subElementRect(sr, opt, widget);
    }
    return r;
}

// for QForm
bool RibbonStyle::drawFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const RibbonMainWindow*>(w))
    {
        QTN_D(const RibbonStyle);
        if (const StyleOptionFrame* optFrame = qstyleoption_cast<const StyleOptionFrame*>(opt))
        {
            bool active = optFrame->active;

            QRect rc = optFrame->rect;
            QRect rcBorders = optFrame->clientRect;

            int nRightBorder = rcBorders.left() - rc.left(), nLeftBorder = rcBorders.left() - rc.left(), nBorder = optFrame->frameBorder;
            int nBottomBorder = rc.bottom() - rcBorders.bottom();

            rc.translate(-rc.topLeft());
            QRect rcFrame(rc);

            int nCaptionHeight = optFrame->titleBarSize;
            rcFrame.setTop(rcFrame.top() + nCaptionHeight);

            int nStatusHeight = optFrame->statusHeight;
            bool bHasStatusBar = optFrame->hasStatusBar;

            int bordersHeight = bHasStatusBar ? rcFrame.height() - nStatusHeight - 1 : rcFrame.height();

            if (nLeftBorder > 0) 
                p->fillRect(rc.left() + 0, rcFrame.top(), 1, rcFrame.height(), active ? m_clrFrameBorderActive0 : m_clrFrameBorderInactive0);
            if (nLeftBorder > 1) 
                p->fillRect(rc.left() + 1, rcFrame.top(), 1, bordersHeight, active ? m_clrFrameBorderActive1 : m_clrFrameBorderInactive1);

            if (nRightBorder > 0) 
                p->fillRect(rc.right() - 1, rcFrame.top(), 1, rcFrame.height(), active ? m_clrFrameBorderActive0 : m_clrFrameBorderInactive0);
            if (nRightBorder > 1) 
                p->fillRect(rc.right() - 2, rcFrame.top(), 1, bordersHeight, active ? m_clrFrameBorderActive1 : m_clrFrameBorderInactive1);

            if (d.flatFrame_)
            {
                if (nLeftBorder > 2) 
                    p->fillRect(rc.left() + 2, rcFrame.top(), nLeftBorder - 3, bordersHeight, active ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);
                if (nLeftBorder > 2) 
                    p->fillRect(rc.left() + nLeftBorder - 1, rcFrame.top(), 1, bordersHeight, active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);

                if (nRightBorder > 2) 
                    p->fillRect(rc.right() - nRightBorder + 1, rcFrame.top(), nRightBorder - 3, bordersHeight, active ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);
                if (nRightBorder > 2) 
                   p->fillRect(rc.right() - nRightBorder, rcFrame.top(), 1, bordersHeight, active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);
            }
            else
            {
                if (nLeftBorder > 2) 
                    p->fillRect(rc.left() + 2, rcFrame.top(), 1, bordersHeight, active ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);
                if (nLeftBorder > 3) 
                    p->fillRect(rc.left() + 3, rcFrame.top(), nLeftBorder - 3, bordersHeight, active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);

                if (nRightBorder > 2) 
                    p->fillRect(rc.right() - 3, rcFrame.top(), 1, bordersHeight, active ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);
                if (nRightBorder > 3) 
                    p->fillRect(rc.right() - nRightBorder, rcFrame.top(), nRightBorder - 3, bordersHeight, active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);
            }

            p->fillRect(rc.left(), rc.bottom() - 1, rc.width(), 1,
                active ? optFrame->maximizeFlags ? m_clrFrameBorderActive3 : m_clrFrameBorderActive0 : m_clrFrameBorderInactive0);

            if (nBottomBorder > 1)
            {
                if (d.flatFrame_)
                {
                    p->fillRect(rc.left() + nLeftBorder, rc.bottom() - nBottomBorder, rc.width() - nLeftBorder - nRightBorder, 1, active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);
                    p->fillRect(rc.left() + 1, rc.bottom() - nBottomBorder + 1, rc.width() - 2, nBottomBorder - 2, active ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);
                }
                else
                {
                    p->fillRect(rc.left() + 1, rc.bottom() - nBottomBorder, rc.width() - 2, nBottomBorder - 1, active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);
                }
            }

            QRect rcCaption(QPoint(rc.left(), rc.top()), QPoint(rc.right(), rc.top() + nCaptionHeight));
            if (optFrame->titleVisible)
            {
                QRect rcTopLeft, rcTopRight, rcTopCenter, rcSrcTopLeft, rcSrcTopRight;

                QPixmap soImage = cached("FrameTopLeft.png");
                if (!soImage.isNull())
                {
                    rcSrcTopLeft = getSource(soImage.rect(), active ? 0 : 1, 2);
                    rcTopLeft = QRect(QPoint(rc.left(), rc.top()), QPoint(rc.left() + rcSrcTopLeft.width(), rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopLeft, rcSrcTopLeft, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopRight.png");
                if (!soImage.isNull())
                {
                    rcSrcTopRight = getSource(soImage.rect(), active ? 0 : 1, 2);
                    rcTopRight = QRect(QPoint(rc.right() - rcSrcTopRight.width() - 1, rc.top()), QPoint(rc.right() - 1, rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopRight, rcSrcTopRight, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopCenter.png");
                if (!soImage.isNull())
                {
                    rcTopCenter = QRect(QPoint(rc.left() + rcTopLeft.width(), rc.top()), QPoint(rc.right() - rcSrcTopRight.width(), rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopCenter, getSource(soImage.rect(), active ? 0 : 1, 2), QRect(QPoint(0, 5), QPoint(0, 3)));
                }
            }
            else
            {
                QRect rcSrc;
                QPixmap soImage = cached("FrameTopLeft.png");
                if (!soImage.isNull())
                {
                    rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                    rcSrc.setRight(nLeftBorder);

                    QRect rcTopLeft(QPoint(rc.left(), rcCaption.top()), QPoint(rc.left() + nLeftBorder, rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopLeft, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopRight.png");
                if (!soImage.isNull())
                {
                    rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                    rcSrc.setLeft(rcSrc.right() - nRightBorder);

                    QRect rcTopRight(QPoint(rc.right() - nRightBorder - 1, rcCaption.top()), QPoint(rc.right()-1, rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopRight, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopCenter.png");
                if (!soImage.isNull())
                {
                    rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                    rcSrc.setBottom(rcSrc.top() + nBorder);

                    QRect rcTopCenter(QPoint(rc.left() + nLeftBorder, rc.top()), QPoint(rc.right() - nRightBorder - 1, rc.top() + nBorder));
                    drawImage(soImage, *p, rcTopCenter, rcSrc);
                }
            }

            if (optFrame->hasStatusBar)
            {
                QRect rcSrc;
                QPixmap soImage;
                if (!d.flatFrame_)
                {
                    soImage = cached("StatusBarLight.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = QRect(QPoint(0, 0), QPoint(nLeftBorder - 1, soImage.height()));
                        QRect rcLight(QPoint(rc.left() + 1, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.left() + nLeftBorder, rc.bottom() - nBottomBorder));
                        drawImage(soImage, *p, rcLight, rcSrc);
                    }
                    soImage = cached("StatusBarDark.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = QRect(QPoint(0, 0), QPoint(nRightBorder - 1, soImage.height()));
                        QRect rcDark(QPoint(rc.right() - nRightBorder, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.right()-2, rc.bottom() - nBottomBorder));
                        drawImage(soImage, *p, rcDark, rcSrc);
                    }
                }
                else
                {
                    QRect rcLight(QPoint(rc.left() + 1, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.left() + nLeftBorder, rc.bottom() - nBottomBorder + 1));
                    p->fillRect(rcLight, active ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);

                    QRect rcDark(QPoint(rc.right() - nRightBorder, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.right() - 2, rc.bottom() - nBottomBorder + 1));
                    p->fillRect(rcDark, active ? m_clrFrameBorderActive2 : m_clrFrameBorderInactive2);

                    if (nBottomBorder > 1)
                    {
                        p->fillRect(QRect(QPoint(rc.left() + nLeftBorder - 1, rc.bottom() - nStatusHeight - nBottomBorder), QSize(1, nStatusHeight)),
                            active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);
                        p->fillRect(QRect(QPoint(rc.right() - nRightBorder, rc.bottom() - nStatusHeight - nBottomBorder), QSize(1, nStatusHeight)),
                            active ? m_clrFrameBorderActive3 : m_clrFrameBorderInactive3);
                    }
                }
            }

            bool roundedCornersAlways = true;
            if (optFrame->hasStatusBar || roundedCornersAlways)
            {
                QRect rcSrc;
                QPixmap soImage;
                if (nLeftBorder > 3)
                {
                    soImage = cached("FrameBottomLeft.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                        QRect rcBottomLeft(QPoint(rc.left(), rc.bottom() - rcSrc.height()), QPoint(rc.left() + rcSrc.width(), rc.bottom()));
                        drawImage(soImage, *p, rcBottomLeft, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                    }
                }

                if (nRightBorder > 3)
                {
                    soImage = cached("FrameBottomRight.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                        QRect rcBottomRight(QPoint(rc.right() - rcSrc.width()-1, rc.bottom() - rcSrc.height()), QPoint(rc.right()-1, rc.bottom()));
                        drawImage(soImage, *p, rcBottomRight, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                    }
                }
            }
            return true;
        }
    }
    return false;
}

// for QForm
bool RibbonStyle::drawShapedFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    bool ret = false;
    if (const QMdiArea* mdiArea = qobject_cast<const QMdiArea*>(w))
    {
        if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt)) 
        {
            int frameShape  = f->frameShape;
            int frameShadow = QFrame::Plain;
            if (f->state & QStyle::State_Sunken)
                frameShadow = QFrame::Sunken;
            else if (f->state & QStyle::State_Raised)
                frameShadow = QFrame::Raised;

            switch (frameShape)
            {
                case QFrame::Panel:
                    {
                        if (frameShadow == QFrame::Sunken)
                        {
                            int lw = f->lineWidth;
                            int mlw = f->midLineWidth;

                            p->fillRect(opt->rect, m_clrHighlightChecked);

                            QRect rect = opt->rect;
                            if (QTabBar* tabBar = ::qFindChild<QTabBar*>(mdiArea))
                            {
                                switch (mdiArea->tabPosition())
                                {
                                    case QTabWidget::North:
                                                rect.adjust(0,tabBar->sizeHint().height()-1,0,0);
                                            break;
                                    case QTabWidget::South:
                                                rect.adjust(0, 0, 0, -tabBar->sizeHint().height()-1);
                                            break;
                                    case QTabWidget::West:
                                                rect.adjust(tabBar->sizeHint().width()-1, 0, 0, 0);
                                            break;
                                    case QTabWidget::East:
                                                rect.adjust(0, 0, -tabBar->sizeHint().width()-1, 0);
                                            break;
                                    default:
                                        break;
                                }
                            }

                            rect.adjust(0, 0, -1, -1);
                            QPen savePen = p->pen();
                            p->setPen(opt->palette.dark().color());
                            p->drawRect(rect);
                            p->setPen(savePen);

                            rect.adjust(2, 2, -1, -1);
                            qDrawShadeRect(p, rect, opt->palette, true, lw, mlw);
                            ret = true;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    else if (qobject_cast<const RibbonBackstageSeparator*>(w))
    {
        if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt)) 
        {
            p->fillRect(opt->rect, QColor(255, 255, 255));

            if (f->frameShape == QFrame::HLine)
            {
                const uchar _cb[] = {0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
                QBitmap cb = QBitmap::fromData(QSize(8, 8), _cb);
                QBrush brush(QColor(199, 200, 201), cb);
                QRect rc(QPoint(opt->rect.left(), opt->rect.height() / 2), QSize(opt->rect.width(), 1));
                p->fillRect(rc, brush);
                ret = true;
            }
            else if (f->frameShape == QFrame::VLine)
            {
                QPixmap soImage = CommonStyle::cached(getPathForBackstageImages() +"BackstagePaneSeparator.png");
                if (!soImage.isNull())
                {
                    QRect rc(QPoint(opt->rect.width()/2 - 1, opt->rect.top()), QSize(2, opt->rect.height()));

                    drawImage(soImage,*p, rc/*QRect(QPoint(opt->rect.right() - 1, opt->rect.top()), 
                        QPoint(opt->rect.right(), opt->rect.bottom()))*/, getSource(soImage.rect(), 0, 1), QRect(QPoint(0, 30), QPoint(0, 30)));
                }
                ret = true;
            }
        }
    }
    return ret;
}

// for stausBar
bool RibbonStyle::drawPanelStatusBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QPixmap soDevider = cached("StatusBarDevider.png");
    const int widthDevider = soDevider.width();
    int nStatusHeight = opt->rect.height();
    QRect rc( opt->rect );
    int bRightOffset = w->childrenRect().width();
    int nWidth = rc.width()-bRightOffset-widthDevider;

    QPixmap soLight = cached("StatusBarLight.png");
    QRect rcSrc = QRect( QPoint(0, 0), QPoint(0, soLight.height()) );
    QRect rcLight(QPoint(rc.left(), rc.top()), QPoint(nWidth, rc.bottom()+1) );
    drawImage( soLight, *p, rcLight, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)) );

    QPixmap soDark = cached("StatusBarDark.png");
    rcSrc = QRect(QPoint(0, 0), QPoint(0, soDark.height()) );
    QRect rcDark( QPoint(nWidth+widthDevider, rc.bottom() - nStatusHeight+1), QPoint(rc.right(), rc.bottom()+1) );
    drawImage( soDark, *p, rcDark, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)) );

    rcSrc = QRect( QPoint(0, 0), QPoint(soDevider.width(), soDevider.height()) );
    QRect rcDevider( QPoint(nWidth, rc.bottom() - nStatusHeight+1), QSize(rcSrc.width(), rc.height()+1) );
    drawImage( soDevider, *p, rcDevider, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)) );
    return true;
}

void RibbonStyle::drawRibbonBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const StyleOptionRibbon* optRibbonBar = qstyleoption_cast<const StyleOptionRibbon*>(opt))
    {
        QTN_D(const RibbonStyle);

        QRect rcRibbonClient = opt->rect;
        if (optRibbonBar->frameHelper)
            rcRibbonClient.setTop(rcRibbonClient.top() + optRibbonBar->titleBarHeight);

        QRect rcRibbonTabs = optRibbonBar->rectTabBar;
        rcRibbonTabs.setLeft(rcRibbonClient.left()); 
        rcRibbonTabs.setRight(rcRibbonClient.right());

        QRect rcRibbonGroups(QPoint(rcRibbonClient.left(), rcRibbonTabs.bottom()), QPoint(rcRibbonClient.right(), rcRibbonClient.bottom()));

        p->fillRect(rcRibbonTabs, m_clrRibbonFace);
        p->fillRect(rcRibbonGroups, m_clrRibbonFace);

        if (d.flatFrame_ && optRibbonBar->frameHelper)
        {
            if (!optRibbonBar->frameHelper->isDwmEnabled())
            {
                if (!optRibbonBar->frameHelper->isActive())
                    optRibbonBar->frameHelper->fillSolidRect(p, rcRibbonTabs, m_clrRibbonInactiveFace);
            }
            else
            {
                optRibbonBar->frameHelper->fillSolidRect(p, rcRibbonTabs, QColor(0, 0, 0));
                QPixmap soImage = cached("RibbonTabBackgroundDwm.png");
                if (!soImage.isNull())
                {
                    QRect rcSrc = getSource(soImage.rect(), 0, 1);
                    QRect rcRibbonClient = opt->rect;
                    QRect rcRibbonTabs = optRibbonBar->rectTabBar;

                    int height = qMin(rcRibbonTabs.height(), rcSrc.height()+1);
                    rcSrc.setTop(rcSrc.bottom() - height);
                    QRect rcArea(QPoint(rcRibbonClient.left(), rcRibbonTabs.bottom() - height), 
                        QPoint(rcRibbonClient.right(), rcRibbonTabs.bottom()));
                    drawImage(soImage, *p, rcArea, rcSrc, QRect(QPoint(10, 0), QPoint(10, 0)), isStyle2010());
                }
            }
        }

        if (optRibbonBar->minimized)
        {
            p->fillRect(rcRibbonClient.left(), rcRibbonClient.bottom() - 2, rcRibbonClient.width(), 1, m_clrMinimizedFrameEdgeShadow);
            p->fillRect(rcRibbonClient.left(), rcRibbonClient.bottom() - 1, rcRibbonClient.width(), 1, m_clrMinimizedFrameEdgeHighLight);
        }
    }
}

QString qtn_getColorName(ContextColor color)
{
    switch (color)
    {
        case ContextColorBlue: 
            return QApplication::tr("Blue");
        case ContextColorYellow: 
            return QApplication::tr("Yellow");
        case ContextColorGreen: 
            return QApplication::tr("Green");
        case ContextColorRed: 
            return QApplication::tr("Red");
        case ContextColorPurple: 
            return QApplication::tr("Purple");
        case ContextColorCyan: 
            return QApplication::tr("Cyan");
        case ContextColorOrange: 
            return QApplication::tr("Orange");
        default:
            break;
    }
    return QApplication::tr("");
}

void RibbonStyle::drawRibbonGroups(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);

    ContextColor pageColor = ContextColorNone;
    if (const RibbonBar* rb = qobject_cast<const RibbonBar*>(widget))
    {
        if (RibbonPage* page = rb->getPage(rb->currentIndexPage()))
            pageColor = page->getContextColor();
    }
    else if (const ReducedGroupPopupMenu* pm = qobject_cast<const ReducedGroupPopupMenu*>(widget))
    {
        if (const RibbonPage* page = getParentWidget<const RibbonPage>(pm->getGroup()))
            pageColor = page->getContextColor();
    }

    if (pageColor != ContextColorNone)
    {
        QString resName = QApplication::tr("ContextPage");
        resName += qtn_getColorName(pageColor);
        resName += QApplication::tr("Client");

        QPixmap soImage = cached(resName);
        if (!soImage.isNull())
            drawImage(soImage, *p, option->rect, getSource(soImage.rect(), 0, 1),
                soImage.width() < 5 ? QRect(QPoint(1, 8), QPoint(1, 8)) : QRect(QPoint(8, 8), QPoint(8, 8)));
    }
    else
    {
        QPixmap soImage = cached("RibbonGroups.png");
        if (!soImage.isNull())
            drawImage(soImage, *p, option->rect, getSource(soImage.rect(), 0, 1), QRect(QPoint(8, 8), QPoint(8, 8)));
    }
}

void RibbonStyle::drawGroup(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (paintAnimation(tp_ControlElement, CE_Group, opt, p, widget, 300))
        return;

    if (const QStyleOptionGroupBox* gr = qstyleoption_cast<const QStyleOptionGroupBox*>(opt))
    {
        QRect rcEntryCaption = opt->rect;
        rcEntryCaption.setTop(rcEntryCaption.bottom() - gr->lineWidth);

        ContextColor pageColor = ContextColorNone;
        if (const RibbonPage* page = getParentWidget<const RibbonPage>(widget))
            pageColor = page->getContextColor();

        QPixmap soImage;
        QPixmap soImageCaption;
        if (pageColor == ContextColorNone)
        {
            soImage = cached("RibbonGroupClient.png");
            soImageCaption = cached("RibbonGroupCaption.png");
        }
        else
        {
            QString resName = QApplication::tr("ContextPage");
            resName += qtn_getColorName(pageColor);
            resName += QApplication::tr("GroupClient.png");

            soImage = cached(resName);

            if (soImage.isNull())
            {
                soImage = cached("ContextPageGroupClient.png");
                soImageCaption = cached("ContextPageGroupCaption.png");
            }
        }

        QRect rcEntry(gr->rect);
        if (!soImageCaption.isNull())
            rcEntry.setBottom(rcEntry.bottom() - gr->lineWidth);

        int state = opt->state & State_MouseOver ? 1 : 0;

        if (!soImage.isNull())
            drawImage(soImage, *p, rcEntry, getSource(soImage.rect(), state, 2), QRect(QPoint(5, 5), QPoint(5, 5)), (state == 1 && isStyle2010()));

        if (!soImageCaption.isNull())
            drawImage(soImageCaption, *p, rcEntryCaption, getSource(soImageCaption.rect(), state, 2), QRect(QPoint(5, 5), QPoint(5, 5)));

        soImage = cached("RibbonGroupSeparator.png");
        if (!soImage.isNull())
        {
            QRect rcGroup(gr->rect);
            drawImage(soImage, *p, QRect(QPoint(rcGroup.right() - 1, rcGroup.top() + 2), QPoint(rcGroup.right() + 1, rcGroup.bottom() - 7)), 
                getSource(soImage.rect()), QRect(QPoint(0, 0), QPoint(0, 0)));
        }
    }
}

void RibbonStyle::drawReducedGroup(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionToolButton* optGroup = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        ContextColor pageColor = ContextColorNone;
        if (const RibbonPage* page = getParentWidget<const RibbonPage>(widget))
            pageColor = page->getContextColor();

        QPixmap soImage;
        if (pageColor != ContextColorNone)
        {
            QString resName = QApplication::tr("ContextPage");
            resName += qtn_getColorName(pageColor);
            resName += QApplication::tr("GroupButton");
            soImage = cached(resName);
        }
        else
            soImage = cached("RibbonGroupButton.png");

        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        QRect rcEntry(opt->rect);
        bool bPopuped  = (optGroup->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);
        int nState = bPopuped ? 2 : (opt->state & State_MouseOver ? 1 : 0);
        QRect rcSrc = getSource(soImage.rect(), nState, 3);
        drawImage(soImage, *p, rcEntry, rcSrc, QRect(QPoint(5, 5), QPoint(5, 5)), (nState == 1 && isStyle2010()));
        // ----
        QFontMetrics fm(optGroup->fontMetrics);
        int heightTxt = fm.height();
        int topOffset = heightTxt+heightTxt/3;
        int flags = Qt::AlignHCenter | Qt::AlignVCenter;

        flags |= Qt::TextShowMnemonic;
        if (!proxy()->styleHint(QStyle::SH_UnderlineShortcut, optGroup, widget))
            flags |= Qt::TextHideMnemonic;

        QString strFirstRow, strSecondRow;
        CommonStyle::splitString(optGroup->text, strFirstRow, strSecondRow);
        
        QRect rectFirstRow = rcEntry;

        if (!strSecondRow.isEmpty())
            rectFirstRow.adjust(0, rectFirstRow.height()/2-heightTxt, 0, -rectFirstRow.height()/2);
        
        proxy()->drawItemText(p, rectFirstRow, flags, optGroup->palette, optGroup->state & State_Enabled, strFirstRow, QPalette::WindowText);
        if (!strSecondRow.isEmpty())
        {
            QRect rectSecondRow = rcEntry;
            rectSecondRow.adjust(0, rectSecondRow.height()/2, 0, -rectSecondRow.height()/2 + heightTxt);
            proxy()->drawItemText(p, rectSecondRow, flags, optGroup->palette, optGroup->state & State_Enabled, strSecondRow, QPalette::WindowText);
        }
        //===========================================================================================
        soImage = cached("ToolbarButtonDropDownGlyph.png");

        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        nState = !(optGroup->state & State_Enabled) ? 3 : opt->state & State_MouseOver ? 1 : 0;
        rcSrc = getSource(soImage.rect(), nState, 4);
        QPoint pt = rcEntry.center();
        pt.setY(pt.y() + topOffset);
        QRect rc(QPoint(pt.x() - 2, pt.y() - 2), rcSrc.size());
        drawImage(soImage, *p, rc, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        //===========================================================================================
    }
}

// for SizeGrip
bool RibbonStyle::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QRect rc(opt->rect);
    QPixmap soGripper = cached("StatusBarGripper.png");
    QRect rcSrc = soGripper.rect();
    QRect rcGripper( QPoint(/*rc.right()-rcSrc.width(), rc.bottom()-rcSrc.height()*/2,-1), QSize(rcSrc.width(), rcSrc.height()));
    drawImage(soGripper, *p, rcGripper, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    return true;
}

void RibbonStyle::drawContextHeaders(const QStyleOption* opt, QPainter* p) const
{
    if (const StyleOptionTitleBar* optTitleBar = qstyleoption_cast<const StyleOptionTitleBar*>(opt))
    {
        const QList<ContextHeader*>& listContextHeaders = *optTitleBar->listContextHeaders;

        int count = listContextHeaders.count();
        if (count == 0)
            return;

        for (int i = 0; i < count; i++)
        {
            ContextHeader* header = listContextHeaders.at(i);

            QString resName = QApplication::tr("ContextTab");
            resName += qtn_getColorName(header->color);
            resName += QApplication::tr("Header");

            QPixmap soImage = cached(resName);
            if (soImage.isNull())
                return;

            QRect rc = header->rcRect;
            drawImage(soImage, *p, rc, soImage.rect(), QRect(QPoint(2, 4), QPoint(2, 2)));

            rc.setLeft(rc.left() + 6);
            int frameBorder = optTitleBar->frameHelper ? optTitleBar->frameHelper->getFrameBorder() : 0;

            rc.setTop(rc.top() + (frameBorder - 2));

            if (optTitleBar->frameHelper && optTitleBar->frameHelper->isDwmEnabled())
                optTitleBar->frameHelper->drawDwmCaptionText(p, rc, header->strTitle, optTitleBar->frameHelper->isActive());
            else
                p->drawText(rc, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, 
                    p->fontMetrics().elidedText(header->strTitle, Qt::ElideRight, rc.width()));
        }
    }
}

// for TitleBar
bool RibbonStyle::drawTitleBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const 
{
    if (!qobject_cast<const RibbonBar*>(w))
        return OfficeStyle::drawTitleBar(opt, p, w);

    if (const StyleOptionTitleBar* optTitleBar = qstyleoption_cast<const StyleOptionTitleBar*>(opt))
    {
        if (optTitleBar->frameHelper)
        {
            QRect rcCaption = optTitleBar->rect;
            bool dwmEnabled = optTitleBar->frameHelper->isDwmEnabled();
            bool active = optTitleBar->state & State_Active;

            QRect rcText = optTitleBar->rcTitleText;
            if (optTitleBar->titleBarState & Qt::WindowMaximized)
                rcText.setTop(rcText.top() + optTitleBar->frameHelper->getFrameBorder() / 2);

            if (!dwmEnabled)
            {
                QRect rcSrc;
                QRect rcTopLeft;
                QPixmap soImage = cached("FrameTopLeft.png");
                if (!soImage.isNull())
                {
                    rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                    rcTopLeft = QRect(QPoint(rcCaption.left(), rcCaption.top()), QPoint(rcCaption.left() + rcSrc.width(), rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopLeft, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
                }

                soImage = cached("FrameTopRight.png");
                QRect rcTopRight;
                if (!soImage.isNull())
                {
                    rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                    rcTopRight = QRect(QPoint(rcCaption.right() - rcSrc.width(), rcCaption.top()), QPoint(rcCaption.right(), rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopRight, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
                }

                soImage = cached("FrameTopCenter.png");
                if (!soImage.isNull())
                {
                    rcSrc = getSource(soImage.rect(), active ? 0 : 1, 2);
                    QRect rcTopCenter(QPoint(rcTopLeft.right(), rcCaption.top()), QPoint(rcTopRight.left(), rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopCenter, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
            }
            else
            {
                optTitleBar->frameHelper->fillSolidRect(p, rcCaption, QColor(0, 0, 0));

                StyleOptionTitleBar optTitle = *optTitleBar;
                optTitle.rcTitleText = rcText;
                optTitleBar->frameHelper->drawTitleBar(p, optTitle);

                QTN_D(const RibbonStyle);
                if (d.flatFrame_)
                {
                    QPixmap soImage = cached("RibbonTabBackgroundDwm.png");
                    if (!soImage.isNull())
                    {
                        QRect rcSrc = getSource(soImage.rect(), 0, 1);
                        QRect rcRibbonClient = optTitleBar->rcRibbonClient;
                        QRect rcRibbonTabs = optTitleBar->rectTabBar;
                        rcSrc.setBottom(rcSrc.bottom() - rcRibbonTabs.height());

                        if (rcSrc.height() > 0)
                        {
                            QRect rcArea(QPoint(rcRibbonClient.left(), rcRibbonTabs.top() - rcSrc.height()), 
                                QPoint(rcRibbonClient.right(), rcRibbonTabs.top()));
                            drawImage(soImage, *p, rcArea, rcSrc, QRect(QPoint(10, 0), QPoint(10, 0)), isStyle2010());
                        }
                    }
                }
            }

            if (optTitleBar->quickAccessOnTop && optTitleBar->quickAccessVisible && optTitleBar->quickAccessVisibleCount > 0)
            {
                if (optTitleBar->existSysButton && !optTitleBar->normalSysButton)
                {
                    QRect rcQuickAccess = optTitleBar->rcQuickAccess;
//                    QRect rcQuickAccessArea(QPoint(rcQuickAccess.left() - 15, rcQuickAccess.top() + 2), QPoint(rcQuickAccess.right(), rcQuickAccess.top() + 2 + rcQuickAccess.height()));
                    QRect rcQuickAccessArea = rcQuickAccess.adjusted(-16, -1, 4, 0);//(QPoint(rcQuickAccess.left() - 15, rcQuickAccess.top()), QPoint(rcQuickAccess.right(), rcQuickAccess.top() + rcQuickAccess.height()));

                    QPixmap soQuickAccessImage = cached(optTitleBar->frameHelper && optTitleBar->frameHelper->isDwmEnabled()?  "RibbonQuickAccessAreaDwm.png" : "RibbonQuickAccessArea.png" );
                    if (!soQuickAccessImage.isNull())
                        drawImage(soQuickAccessImage, *p, rcQuickAccessArea, soQuickAccessImage.rect(), QRect(QPoint(16, 3), QPoint(16, 3)));
                }
            }

            if ((!optTitleBar->existSysButton || optTitleBar->normalSysButton) && optTitleBar->frameHelper)
            {
                if (!optTitleBar->icon.isNull())
                {
                    int frameBorder = optTitleBar->frameHelper->getFrameBorder();
                    int topBorder = optTitleBar->frameHelper->isDwmEnabled() ? optTitleBar->frameHelper->getFrameBorder() : 0;

//                    QSize szIcon = proxy()->subControlRect(CC_TitleBar, optTitleBar, SC_TitleBarSysMenu, w).size();
//                    QSize szIcon = optTitleBar->icon.actualSize(QSize(64, 64));
                    QSize szIcon = optTitleBar->frameHelper->getSizeSystemIcon();

                    int top = topBorder / 2 - 1 + (rcCaption.bottom()  - szIcon.height())/2;
                    int left = rcCaption.left() + frameBorder + 1;

                    QPixmap soSeparatorImage = cached("SystemIconSeparator.png");
                    if (!soSeparatorImage.isNull())
                    {
                        QRect rcSrc = soSeparatorImage.rect();
                        QRect rcDest(QPoint(left + szIcon.height() + 6, top + szIcon.height() / 2 - rcSrc.height() / 2), rcSrc.size());
                        drawImage(soSeparatorImage, *p, rcDest, soSeparatorImage.rect());
                    }
                }
            }

            if (optTitleBar->listContextHeaders && optTitleBar->listContextHeaders->count() > 0)
                proxy()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonContextHeaders, opt, p, w);

            if (!dwmEnabled)
            {
                QPen savePen = p->pen();
                p->setPen(!active ? m_clrFrameCaptionTextInActive : m_clrFrameCaptionTextActive);
                QString text = p->fontMetrics().elidedText(optTitleBar->text, Qt::ElideRight, rcText.width());
                p->drawText(rcText, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, text);
                p->setPen(savePen);
            }

            if (optTitleBar->drawIcon && optTitleBar->frameHelper)
            {
                if (!dwmEnabled && optTitleBar->subControls & SC_TitleBarSysMenu && optTitleBar->titleBarFlags & Qt::WindowSystemMenuHint) 
                {
                    QStyleOption tool(0);
                    tool.palette = optTitleBar->palette;

                    QRect ir = proxy()->subControlRect(CC_TitleBar, optTitleBar, SC_TitleBarSysMenu, w);
                    ir.translate(4, 0);

                    if (!optTitleBar->icon.isNull())
                    {
                        optTitleBar->icon.paint(p, ir);
                    }
                    else
                    {
                        int iconSize = proxy()->pixelMetric(PM_SmallIconSize, optTitleBar, w);
                        QPixmap pm = standardIcon(SP_TitleBarMenuButton, &tool, w).pixmap(iconSize, iconSize);
                        tool.rect = ir;
                        p->save();
                        proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                        p->restore();
                    }
                }
            }
            return true;
        }
        else
        {
            QRect rcTitle = optTitleBar->rect;
            rcTitle.setTop(rcTitle.top()-1);

            QPixmap soImage = cached("FrameTopCenter.png");
            if (!soImage.isNull())
            {
                QRect rcSrc = getSource(soImage.rect(), 0, 2);
                drawImage(soImage, *p, rcTitle, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
            }

            if (optTitleBar->quickAccessOnTop && optTitleBar->quickAccessVisible && optTitleBar->quickAccessVisibleCount > 0)
            {
                if (optTitleBar->existSysButton && !optTitleBar->normalSysButton)
                {
                    QRect rcQuickAccess = optTitleBar->rcQuickAccess;
                    QRect rcQuickAccessArea = rcQuickAccess.adjusted(-16, -1, 4, 0);//(QPoint(rcQuickAccess.left() - 15, 0), QPoint(rcQuickAccess.right(), rcQuickAccess.height()));

                    QPixmap soQuickAccessImage = cached(optTitleBar->frameHelper && optTitleBar->frameHelper->isDwmEnabled()?  "RibbonQuickAccessAreaDwm.png" : "RibbonQuickAccessArea.png" );
                    if (!soQuickAccessImage.isNull())
                        drawImage(soQuickAccessImage, *p, rcQuickAccessArea, soQuickAccessImage.rect(), QRect(QPoint(16, 3), QPoint(16, 3)));
                }
            }

            if (optTitleBar->listContextHeaders && optTitleBar->listContextHeaders->count() > 0)
                proxy()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonContextHeaders, opt, p, w);
            return true;
        }
    }
    return false;
}

bool RibbonStyle::drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const RibbonSystemPopupBar*>(w)) 
    {
        QPixmap soRibbonSystemMenu = cached("RibbonSystemMenu.png");
        drawImage(soRibbonSystemMenu, *p, opt->rect, soRibbonSystemMenu.rect(), QRect(6, 18, 6, 29));
        return true;
    }
//    else if (qobject_cast<const ReducedGroupPopupMenu*>(w)) 
//        drawRibbonBar(opt, p, w);
    return false;
}

bool RibbonStyle::drawSlider(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w)  const
{
    if (!qobject_cast<RibbonSliderPane*>(w->parentWidget()))
        return OfficeStyle::drawSlider(opt, p, w);

    if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        if (slider->orientation == Qt::Horizontal)
        {
            QRect groove = proxy()->subControlRect(CC_Slider, opt, SC_SliderGroove, w);
            QRect handle = proxy()->subControlRect(CC_Slider, opt, SC_SliderHandle, w);

            QPixmap soTrack = cached("SliderTrack.png");
            QRect rcTrackDest(QPoint(groove.left(), (groove.top() + groove.bottom() - soTrack.height()) / 2), 
            QSize(groove.width(), soTrack.height()));
            p->drawPixmap( rcTrackDest, soTrack, soTrack.rect() );

            if (slider->maximum >= slider->minimum) 
            {
                QPixmap soTick = cached("SliderTick.png");
                QRect rcTrackTickDest( QPoint((groove.left() + groove.right() - soTick.width()) / 2,
                    (groove.top() + groove.bottom() - soTick.height()) / 2), soTick.size() );
                drawImage(soTick, *p, rcTrackTickDest, soTick.rect(), QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            }

            if ((opt->subControls & SC_SliderHandle)) 
            {
                QPixmap hndl = cached("SliderThumb.png");
                bool selected = slider->activeSubControls & SC_SliderHandle && opt->state & State_MouseOver;
                bool nPressetHt = slider->state & QStyle::State_Sunken;

                QRect rcSrc = getSource( hndl.rect(), nPressetHt ? 2 : selected ? 1 : 0, 3);
                QPoint point((handle.left() + handle.right() - rcSrc.width()) / 2, (handle.top() + handle.bottom() - rcSrc.height()) / 2);
                p->drawPixmap(point, hndl, rcSrc);
            }
            return true;
        }
    }
    return false;
}

void RibbonStyle::drawSystemButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    if (paintAnimation(tp_PrimitiveElement, PE_RibbonFileButton, option, p, widget))
        return;

    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(option))
    {
        if (qobject_cast<const RibbonBar*>(widget->parentWidget()))
        {
            if (toolbutton->toolButtonStyle != Qt::ToolButtonFollowStyle)
            {
                QPixmap soRibbonFileButton = cached("FileButton.png");
                QTN_D(const RibbonStyle);
                bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (option->state & State_Sunken);
                bool selected = option->state & State_MouseOver;
                bool enabled = widget->isEnabled();

                if (d.fileButtonImageCount_ == 7)
                {
                    int state = !enabled ? 5 : popuped ? (selected ? 3 : 2) :
                        /*isKeyboardSelected(widget)*/false ? 4 : selected ? 1 : 0;

                    QRect rcSrc = getSource(soRibbonFileButton.rect(), state, 7);
                    QRect rcDest = toolbutton->rect.adjusted(0,1,0,0);
                    drawImage(soRibbonFileButton, *p, rcDest, rcSrc, QRect(QPoint(3, 3), QPoint(3, 3)));
                }
                else
                {
                    int state = popuped ? 2 : selected ? 1 :  0;

                    QRect rcSrc = getSource(soRibbonFileButton.rect(), state, 3);
                    drawImage(soRibbonFileButton, *p, toolbutton->rect.adjusted(0,1,0,0), rcSrc, QRect(QPoint(3, 3), QPoint(3, 3)));
                }

                if (toolbutton->toolButtonStyle == Qt::ToolButtonIconOnly && !toolbutton->icon.isNull())
                {
                    bool enabled  = toolbutton->state & State_Enabled;
                    QRect rc = toolbutton->rect;
                    QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize, 
                        enabled ? QIcon::Normal : QIcon::Disabled, selected ? QIcon::On : QIcon::Off);
                    QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
                    p->drawPixmap(ptIcon, pxIcon);
                }
                return;
            }
        }

        QPixmap soImage;
        if (toolbutton->rect.width() > 54 && toolbutton->rect.height() > 54)
            soImage = cached("FrameSystemButton52.png");
        if (soImage.isNull())
            soImage = cached("FrameSystemButton.png");

        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        QRect rc = toolbutton->rect;
        int offset = 0;
        if (rc.top() < 0)
        {
            offset = rc.top();
            rc.translate(0, -offset);
        }

        bool enabled  = toolbutton->state & State_Enabled;
        bool selected = toolbutton->state & State_MouseOver;
        bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (toolbutton->state & State_Sunken);

        int state = 0;
        if (popuped)
            state = 4;
        else if (selected)
            state = 2;
        else if (!enabled)
            state = 1;

        QRect rcSrc(getSource(soImage.rect(), state == 0 ? 0 : state == 2 ? 1 : 2, 3));
        QSize szDest = rcSrc.size();
        QPoint ptDest((rc.left() + rc.right() - szDest.width()) / 2, offset + (rc.top() + rc.bottom() - szDest.height()) / 2);
        drawImage(soImage, *p, QRect(ptDest, szDest), rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)));

        if (!toolbutton->icon.isNull())
        {
            QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize, 
                enabled ? QIcon::Normal : QIcon::Disabled, selected ? QIcon::On : QIcon::Off);
            QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, offset + (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
            p->drawPixmap(ptIcon.x(), ptIcon.y(), pxIcon);
        }
    }
}

void RibbonStyle::drawOptionButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    QRect rc = option->rect;

    QPixmap soRibbonOptionButton = cached("RibbonOptionButton.png");

    if(soRibbonOptionButton.isNull())
        return;

    bool selected = option->state & State_MouseOver;
    bool pressed = option->state & State_Sunken;
    bool enabled = option->state & State_Enabled;

    int state = 0;
    if (!enabled) state = 3;
    else if (pressed) state = 2;
    else if (selected) state = 1;

    QRect rcSrc = getSource(soRibbonOptionButton.rect(), state, 4);
    drawImage(soRibbonOptionButton, *p, option->rect, rcSrc, QRect(QPoint(3, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
}

void RibbonStyle::drawFileButtonLabel(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(option)) 
    {
        int alignment = Qt::TextShowMnemonic;
        if (!proxy()->styleHint(SH_UnderlineShortcut, toolbutton, widget))
            alignment |= Qt::TextHideMnemonic;

        alignment |= Qt::AlignCenter;
        alignment |= Qt::TextSingleLine;

        proxy()->drawItemText(p, toolbutton->rect, alignment, toolbutton->palette,
            toolbutton->state & State_Enabled, toolbutton->text, QPalette::ButtonText);
    }
}

void RibbonStyle::drawPopupSystemButton(const QStyleOption* option, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) 
    {
        QPixmap soRibbonSystemMenu = cached("RibbonSystemMenuButton.png");
        QRect rcSrc = getSource(soRibbonSystemMenu.rect(), toolbutton->state & State_MouseOver ? 1 : 0, 2);
        drawImage(soRibbonSystemMenu, *p, toolbutton->rect, rcSrc, QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
    }
}

void RibbonStyle::drawQuickAccessButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
    {
        QPixmap soQuickButton = cached("RibbonQuickAccessButton.png");
        Q_ASSERT(!soQuickButton.isNull());
        if (soQuickButton.isNull())
            return;

        bool selected = opt->state & State_MouseOver;
        bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);

        drawImage(soQuickButton, *p, toolbutton->rect, getSource(soQuickButton.rect(), !w->isEnabled() ? 4 : popuped ? 2 : selected ? 1  : 0, 5), 
            QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
    }
}

void RibbonStyle::drawPopupResizeGripper(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionSizeGrip* sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip*>(option)) 
    {
        QPixmap soImage = cached("PopupBarResizeGripperFace.png");
        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        drawImage(soImage, *p, sizeGrip->rect, soImage.rect(), QRect(QPoint(1, 1), QPoint(1, 1)), QColor(0xFF, 0, 0xFF));

        soImage = cached("PopupBarResizeGripperWidth.png");

        QRect rc = soImage.rect();
        QRect rcDest(QPoint(sizeGrip->rect.right() - rc.width(), sizeGrip->rect.bottom() - rc.height()), rc.size());
        drawImage(soImage, *p, rcDest, rc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    }
}

bool RibbonStyle::drawMenuItem(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (qobject_cast<const RibbonBackstageView*>(widget))
    {
        if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) 
        {
            bool act = menuitem->state & State_Selected;
            bool focus = menuitem->state & State_HasFocus;
            bool dis = !(menuitem->state & State_Enabled);

            QPalette pal = menuitem->palette;
            QColor clrText = focus ? QColor(Qt::white) : helper().getColor(tr("Ribbon"), tr("BackstageMenuTextColor"), QColor(Qt::black));
            pal.setColor(QPalette::WindowText, clrText);

            QRect rcText(opt->rect);
            rcText.adjust(15, 2, 2, 2);

            if (menuitem->menuItemType == QStyleOptionMenuItem::Normal)
            {
                if (act || focus)
                {
                    QPixmap soImage = cached("BackstageMenuCommand.png");
                    if (!soImage.isNull())
                        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), 0, 3), QRect(QPoint(2, 2), QPoint(2, 2)));
                }
            }
            else if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
            {
                if (act || focus)
                {
                    QPixmap soImage = cached("BackstageMenuPage.png");
                    if (!soImage.isNull())
                        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), focus ? 1 : 0, 4), QRect(QPoint(2, 2), QPoint(2, 2)));

                    if (focus)
                    {
                        soImage = cached("BackstageMenuPageGlyph.png");
                        if (!soImage.isNull())
                        {
                            QRect rcSrc = soImage.rect();
                            QRect rcGlyph(QPoint(opt->rect.right() - rcSrc.width() + 1, (opt->rect.top() + opt->rect.bottom() - rcSrc.height()) / 2), rcSrc.size());
                            p->drawPixmap(rcGlyph, soImage, rcSrc);
                        }
                    }
                }
            }

            int iconWidth = 0;
            if (!menuitem->icon.isNull()) 
            {
                int iconSize = pixelMetric(PM_LargeIconSize, opt, widget);
                if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                    iconSize = pixelMetric(PM_SmallIconSize, opt, widget);

                QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
                QPixmap pixmap = menuitem->icon.pixmap(iconSize, mode);
                QPoint pmr(rcText.left(), (rcText.top() + rcText.bottom() - pixmap.height()) / 2);
                p->drawPixmap(pmr, pixmap);
                iconWidth = pixmap.width();
            }

            rcText.setLeft(rcText.left() + iconWidth + 7);

            int flags = 0;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;
            if (!menuitem->text.isEmpty())
            {
                int index = menuitem->text.indexOf(QLatin1String("&"));
                if (index != -1)
                {
                    index = menuitem->text.indexOf(QLatin1String(" "), index);
                    if (index != -1)
                        flags &= ~Qt::TextHideMnemonic;
                }
            }

            // draw text
            proxy()->drawItemText(p, rcText, flags, pal, opt->state & State_Enabled, menuitem->text, QPalette::WindowText);

        }
        return true;
    }
    else
        return OfficeStyle::drawMenuItem(opt, p, widget);
}

void RibbonStyle::fillRibbonTabControl(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
    {
        if (ContextHeader* contextHeader = tab->getContextHeader())
        {
            if (!contextHeader || !contextHeader->firstTab || !contextHeader->lastTab || contextHeader->color == ContextColorNone)
                return;

            QRect rcFirst(contextHeader->firstTab->rect());
            QRect rcLast(contextHeader->lastTab->rect());

            QString resName = QApplication::tr("ContextTab");
            resName += qtn_getColorName(contextHeader->color);
            resName += "Background.png";
            QPixmap soImage = cached(resName);
            if (!soImage.isNull())
            {
                drawImage(soImage, *p, QRect(rcFirst.left(), opt->rect.top(), rcLast.right()+1, rcLast.bottom()), 
                    soImage.rect(), QRect(QPoint(2, 2), QPoint(2, 2)));
            }
        }
    }
}

void RibbonStyle::drawTabShape(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (paintAnimation(tp_PrimitiveElement, PE_RibbonTab, opt, p, widget))
        return;

    ContextColor pageColor = ContextColorNone;
    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
        pageColor = tab->getContextColor();

    bool selected = opt->state & State_Selected;
    bool highlighted = opt->state & State_MouseOver;
    bool popuped  = (opt->state & QStyle::State_Selected) && (opt->state & State_Sunken);
    bool focused = false;

    if (const RibbonBar* rb = getParentWidget<RibbonBar>(widget))
    {
        if (selected && rb->isMinimized())
        {
            if (popuped)
            {
                focused = false;
                highlighted = false;
            }
            else
            {
                focused = false;
                selected = false;
            }
        }
    }

    if (pageColor != ContextColorNone)
    {
        int state = 0;
        if (selected || highlighted)
        {
            QString resName = QApplication::tr("ContextTab");
            resName += qtn_getColorName(pageColor);
            QPixmap soImage = cached(resName);
            if (!soImage.isNull() && soImage.height() < 70)
            {
                if (selected)
                    state = focused ? 2 : 1;
                else if (highlighted)
                    state = 0;
                drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), state, 3), QRect(QPoint(5, 5), QPoint(5, 5)));
            }
            else
            {
                if (selected)
                    state = focused || highlighted ? 4 : 3;
                else if (highlighted)
                    state = 1;

                if (state == 4)
                {
                    soImage = cached("RibbonTab.png");
                    if(!soImage.isNull())
                        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), 5, 6), 
                            QRect(QPoint(5, 5), QPoint(5, 5)), QColor(0xFF, 0, 0xFF));
                }
                else
                {
                    if (!soImage.isNull()) 
                        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), state, 5), QRect(QPoint(5, 5), QPoint(5, 5)));
                }
            }
        }
    }
    else
    {
        if (selected || highlighted)
        {
            int state = 0;
            if (selected && focused)
                state = 5;
            else if (selected && highlighted)
                state = 4;
            else if (selected)
                state = 3;
            else if (opt->state & QStyle::State_Sunken)
                state = 2;
            else if (highlighted)
                state = 1;
            QPixmap soImage = cached("RibbonTab.png");
            if(!soImage.isNull())
                drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), state, 6), 
                    QRect(QPoint(5, 5), QPoint(5, 5)), QColor(0xFF, 0, 0xFF));
        }
    }

    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
    {
        if (pageColor != ContextColorNone && tab->getContextHeader() && tab->getContextHeader()->firstTab == tab)
        {
            QRect rcSeparator(QPoint(opt->rect.left()-1, opt->rect.top()), QPoint(opt->rect.left(), opt->rect.bottom() - 2));

            QPixmap soImage = cached("ContextTabSeparator.png");
            if (!soImage.isNull()) 
                drawImage(soImage, *p, rcSeparator, soImage.rect());
        }

        if (pageColor!= ContextColorNone && tab->getContextHeader() && tab->getContextHeader()->lastTab == tab)
        {
            QRect rcSeparator(QPoint(opt->rect.right(), opt->rect.top()), QPoint(opt->rect.right() + 1, opt->rect.bottom() - 2));
            QPixmap soImage = cached("ContextTabSeparator.png");
            if (!soImage.isNull()) 
                drawImage(soImage, *p, rcSeparator, soImage.rect());
        }
    }
//    fillRibbonTabControl(opt, p, widget);
}

void RibbonStyle::drawTabShapeLabel(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionHeader * optTab = qstyleoption_cast<const QStyleOptionHeader*>(opt)) 
    {
        bool selected = opt->state & State_Selected;
        bool highlighted = opt->state & State_MouseOver;
        int flags = optTab->textAlignment;

        flags |= Qt::TextShowMnemonic;
        if (!proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
            flags |= Qt::TextHideMnemonic;

        QColor textColor = m_clrTabNormalText;
        if ( selected || highlighted ) 
        {
            if ( selected && highlighted ) 
                textColor = m_clrSelectedText;
            else if ( selected ) 
                textColor = m_clrSelectedText;
        }
        // draw text
        QPalette pal = optTab->palette;
        pal.setColor(QPalette::WindowText, textColor);
        proxy()->drawItemText(p, optTab->rect, flags, pal, opt->state & State_Enabled, optTab->text, QPalette::WindowText);

        fillRibbonTabControl(opt, p, widget);
    }
}

bool RibbonStyle::drawTabBarTabShape(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (!qobject_cast<QMdiArea*>(widget->parentWidget()))
        return OfficeStyle::drawTabBarTabShape(opt, p, widget);

    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(opt)) 
    {
        QPixmap soImage = cached("AccessTab.png"); 
        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        QRect rect = opt->rect;

        bool selected = tab->state & State_Selected;
        bool highlight = opt->state & State_MouseOver;

        int state = 0;
        if (selected && highlight)
            state = 3;
        else if (selected)
            state = 2;
        else if (highlight)
            state = 1;

        QSize sz;
        qreal angle = 0;
        switch (tab->shape) 
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
                {
                    rect.adjust(0, 0, 11, 0);
                    sz = QSize(rect.width(), rect.height());
                    break;
                }
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
                {
                    if (tab->position != QStyleOptionTab::Beginning && tab->position != QStyleOptionTab::OnlyOneTab )
                        rect.adjust(-11, 0, 0, 0);

                    sz = QSize(rect.width(), rect.height());
                    angle = -180;
                    break;
                }
            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
                {
                    if (tab->position != QStyleOptionTab::Beginning && tab->position != QStyleOptionTab::OnlyOneTab )
                        rect.adjust(0, -11, 0, 0);
                    sz = QSize(rect.height(), rect.width());
                    angle = -90;
                    break;
                }
            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
                {
                    if (tab->position != QStyleOptionTab::Beginning && tab->position != QStyleOptionTab::OnlyOneTab )
                        rect.adjust(0, -11, 0, 0);
                    sz = QSize(rect.height(), rect.width());
                    angle = 90;
                    break;
                }
            default:
                break;
        }

        QPixmap soImageRotate(sz);
        soImageRotate.fill(QColor(0xFF, 0, 0xFF));

        QPainter painter(&soImageRotate);
        int heightMap = sz.height()/7;
        drawImage(soImage, painter, QRect(QPoint(0, 0), sz), getSource(soImage.rect(), state, 4),
            QRect(QPoint(5, heightMap), QPoint(22, heightMap)));

        QPixmap resultImage = soImageRotate.transformed(QTransform().rotate(angle));
        resultImage.setMask(resultImage.createMaskFromColor(QColor(0xFF, 0, 0xFF)));

        p->drawPixmap(rect, resultImage);
        return true;
    }
    return false;
}

bool RibbonStyle::drawTabBarTabLabel(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D(const RibbonStyle);

    if (!qobject_cast<QMdiArea*>(widget->parentWidget()))
        return OfficeStyle::drawTabBarTabLabel(opt, p, widget);

    if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab*>(opt))
    {
        QStyleOptionTabV3 tabV2(*tab);
        QRect tr = tabV2.rect;
        bool verticalTabs = tabV2.shape == QTabBar::RoundedEast
            || tabV2.shape == QTabBar::RoundedWest
            || tabV2.shape == QTabBar::TriangularEast
            || tabV2.shape == QTabBar::TriangularWest;

        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
        if (!proxy()->styleHint(SH_UnderlineShortcut, opt, widget))
            alignment |= Qt::TextHideMnemonic;

        if (verticalTabs) 
        {
            p->save();
            int newX, newY, newRot;
            if (tabV2.shape == QTabBar::RoundedEast || tabV2.shape == QTabBar::TriangularEast)
            {
                newX = tr.width() + tr.x();
                newY = tr.y();
                newRot = 90;
            } 
            else 
            {
                newX = tr.x();
                newY = tr.y() + tr.height();
                newRot = -90;
            }
            QTransform m = QTransform::fromTranslate(newX, newY);
            m.rotate(newRot);
            p->setTransform(m, true);
        }
        QRect iconRect;
        d.tabLayout(&tabV2, widget, &tr, &iconRect);
        //we compute tr twice because the style may override subElementRect
        tr = proxy()->subElementRect(SE_TabBarTabText, opt, widget); 

        if (!tabV2.icon.isNull()) 
        {
            QPixmap tabIcon = tabV2.icon.pixmap(tabV2.iconSize,
                (tabV2.state & State_Enabled) ? QIcon::Normal
                : QIcon::Disabled,
                (tabV2.state & State_Selected) ? QIcon::On
                : QIcon::Off);
            p->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
        }
/*
        QFont fn = p->font();
        if (tabV2.state & State_Selected)
        {
            fn.setBold(true);
            p->setFont(fn);
        }
*/
        proxy()->drawItemText(p, tr, alignment, tab->palette, tab->state & State_Enabled, tab->text, QPalette::WindowText);

//        p->setFont(fn);

        if (verticalTabs)
            p->restore();

        if (tabV2.state & State_HasFocus) 
        {
            const int OFFSET = 1 + pixelMetric(PM_DefaultFrameWidth);

            int x1, x2;
            x1 = tabV2.rect.left();
            x2 = tabV2.rect.right() - 1;

            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*tab);
            fropt.rect.setRect(x1 + 1 + OFFSET, tabV2.rect.y() + OFFSET,
                x2 - x1 - 2*OFFSET, tabV2.rect.height() - 2*OFFSET);
            drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
        }
        return true;
    }
    return false;
}

bool RibbonStyle::drawPanelButtonTool(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const RibbonBackstageButton*>(w))
    {
        drawPanelBackstageButton(opt, p, w);
        return true;
    }

    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
    {
        bool smallSize = opt->rect.height() < 33;

        bool enabled  = opt->state & State_Enabled;
        bool checked  = opt->state & State_On;
        bool selected = opt->state & State_MouseOver;
        bool mouseInSplit = opt->state & State_MouseOver && toolbutton->activeSubControls & SC_ToolButton;
        bool mouseInSplitDropDown = opt->state & State_MouseOver && toolbutton->activeSubControls & SC_ToolButtonMenu;
        bool pressed  = opt->state & State_Sunken;
        bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);

        if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup))
        {
            int state = -1;
            if ( !enabled ) state = -1;

            else if (popuped) state = 2;
            else if (checked && !selected && !pressed) state = 2;
            else if (checked && selected && !pressed) state = 3;
            else if (selected && pressed) state = 1;
            else if (selected || pressed) state = 0;

            QPixmap soButton = smallSize ? cached("ToolbarButtons22.png") : cached("ToolbarButtons50.png");
            drawImage(soButton, *p, opt->rect, getSource(soButton.rect(), state, 4), QRect(QPoint(8, 8), QPoint(8, 8)));
            return true;
        }

//        const RibbonGroup* rg = qobject_cast<const RibbonGroup*>(w ? w->parentWidget() : Q_NULL);
        const RibbonGroup* rg = w ? getParentWidget<const RibbonGroup>(w) : Q_NULL;
        if (!rg)
            return false;

        int specialOffset = 0;
        QPixmap soImageSplit, soImageSplitDropDown;
        if (rg && w->property(qtn_Prop_Group).toBool() && rg->isControlsGrouping())
        {
            bool beginGroup = w->property(qtn_Prop_Begin_Group).toBool();
            bool wrap       = w->property(qtn_Prop_Wrap).toBool();
            bool left       = beginGroup || wrap;

            int indexNext = rg->getNextIndex(w);
            QWidget* nextWidget = rg->getWidget(indexNext);
            bool beginGroupNext = nextWidget->property(qtn_Prop_Begin_Group).toBool();
            bool wrapNext       = nextWidget->property(qtn_Prop_Wrap).toBool();

            bool right = indexNext <= rg->getIndexWidget(w)|| beginGroupNext || wrapNext;

            soImageSplit = left ? cached("ToolbarButtonsSpecialSplitLeft.png") :
                cached("ToolbarButtonsSpecialSplitCenter.png");

            soImageSplitDropDown = right ? cached("ToolbarButtonsSpecialSplitDropDownRight.png") :
                cached("ToolbarButtonsSpecialSplitDropDownCenter.png");
            specialOffset = 1;
        }
        else
        {
            soImageSplit = smallSize ? cached("ToolbarButtonsSplit22.png") 
                : cached("ToolbarButtonsSplit50.png");
            soImageSplitDropDown = smallSize ? cached("ToolbarButtonsSplitDropDown22.png") 
                : cached("ToolbarButtonsSplitDropDown50.png");
        }

        if(soImageSplit.isNull() || soImageSplitDropDown.isNull())
            return false;

        QRect rcButton = opt->rect;
        QRect popupr = subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, w);

        QRect rcSplit = smallSize ? QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right() - popupr.width()-2, rcButton.bottom())) 
            : QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right(), rcButton.bottom() - popupr.height()-2));

        int stateSplit = -1;

        if (/*enabledSplit && (selected || pressed || popuped)*/false)
        {
            stateSplit = 4;
        }
        else if (!enabled)
        {
//            if (isKeyboardSelected(selected)) stateSplit = 5;
        }
        else if (checked)
        {
            if (popuped) stateSplit = 5;
            else if (!selected && !pressed) stateSplit = 2;
            else if (selected && !pressed) stateSplit = 3;
            else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) stateSplit = 1;
            else if (pressed) stateSplit = 2;
            else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
        }
        else
        {
            if (popuped) stateSplit = 5;
            else if (/*isKeyboardSelected(bPressed) ||*/ (selected && pressed)) stateSplit = 1;
            else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
        }
        stateSplit += specialOffset;

        if (stateSplit != -1)
        {
            if ( smallSize)
                drawImage(soImageSplit, *p, rcSplit, getSource(soImageSplit.rect(), stateSplit, 6+specialOffset), 
                    QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
            else
                drawImage(soImageSplit, *p, rcSplit, getSource(soImageSplit.rect(), stateSplit, 6+specialOffset), 
                    QRect(QPoint(2, 2), QPoint(2, 2)));
        }

        QRect rcSplitDropDown = smallSize ? QRect(QPoint(rcButton.right() - popupr.width()-1, rcButton.top()), QPoint(rcButton.right(), rcButton.bottom())) :
            QRect(QPoint(rcButton.left(), rcButton.bottom() - popupr.height()-1), QPoint(rcButton.right(), rcButton.bottom()));

        int stateSplitDropDown = -1;

        if (/*enabledDropDown && (selected || pressed || popuped)*/false)
        {
            stateSplitDropDown = 3;
        }
        else if (!enabled)
        {
//            if (isKeyboardSelected(selected)) stateSplitDropDown = 4;
        }
        else if (checked)
        {
            if (popuped) stateSplitDropDown = 2;
            else if (!selected && !pressed) stateSplitDropDown = 2;
            else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) stateSplitDropDown = 0;
            else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
            else stateSplitDropDown = 4;
        }
        else
        {
            if (popuped) stateSplitDropDown = 2;
            else if (/*isKeyboardSelected(bPressed) ||*/ (selected && pressed)) stateSplitDropDown = 0;
            else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
        }

        stateSplitDropDown += specialOffset;
        if (stateSplitDropDown != -1)
        {
            if (smallSize)
                drawImage(soImageSplitDropDown, *p, rcSplitDropDown, getSource(soImageSplitDropDown.rect(), stateSplitDropDown, 5+specialOffset), 
                    QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
            else
                drawImage(soImageSplitDropDown, *p, rcSplitDropDown, getSource(soImageSplitDropDown.rect(), stateSplitDropDown, 5+specialOffset), 
                    QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
        }
        return true;
    }
    else if (const StyleOptionTitleBar* toolbutton = qstyleoption_cast<const StyleOptionTitleBar*>(opt)) 
    {
        bool down = opt->state & State_Sunken;
        bool select = opt->state & State_MouseOver;
        bool active = opt->state & State_Active;
        int state = !active ? 3 : down && select ? 2 : select || down ? 1 : 0;

        if (down || select)
        {
            QPixmap soImageButton;
            if (toolbutton->activeSubControls == SC_TitleBarCloseButton)
                soImageButton = cached("FrameTitleCloseButton.png");

            if (soImageButton.isNull())
                soImageButton = cached("FrameTitleButton.png");

            drawImage(soImageButton, *p, toolbutton->rect, getSource(soImageButton.rect(), down && select ? 1 : 0, 2),
                QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
        }

        QString strGlyphSize = QObject::tr("23.png");
        if (toolbutton->rect.width() < 27) strGlyphSize = QObject::tr("17.png");
        if (toolbutton->rect.width() < 15) strGlyphSize = QObject::tr("13.png");

        QString strButton;
        if (toolbutton->activeSubControls == SC_TitleBarCloseButton)
            strButton = QObject::tr("FrameTitleClose");
        else if (toolbutton->activeSubControls == SC_TitleBarMaxButton)
            strButton = QObject::tr("FrameTitleMaximize");
        else if (toolbutton->activeSubControls == SC_TitleBarMinButton)
            strButton = QObject::tr("FrameTitleMinimize");
        else if (toolbutton->activeSubControls == SC_TitleBarNormalButton)
            strButton = QObject::tr("FrameTitleRestore");

        strButton += strGlyphSize;

        QPixmap soImageIndicator  = cached(strButton);
        if (!soImageIndicator.isNull())
        {
            QRect rcSrc = getSource(soImageIndicator.rect(), state, 5);
            QRect rcGlyph(QPoint((toolbutton->rect.right() + toolbutton->rect.left() - rcSrc.width()+1) / 2, (toolbutton->rect.top() + toolbutton->rect.bottom() - rcSrc.height()+2) / 2), rcSrc.size());
            drawImage(soImageIndicator, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        }
        return true;
    }

    return false;
}

void RibbonStyle::calcRects(const QRect& rc, const QSize& pmSize, const QFontMetrics& fMetric, const QString& text, 
    bool hasMenu, QRect* pmRect, QRect* strFirsRect, QRect* strSecondRect) const
{
    QRect rect(QPoint(0,0), pmSize);
    *pmRect = rect;

    QString strFirstRow, strSecondRow;
    splitString(text, strFirstRow, strSecondRow);

    if (!strFirstRow.isEmpty())
    {
        QSize size = fMetric.size(Qt::TextSingleLine, strFirstRow);
        if (!size.isEmpty())
        {
            if (hasMenu && strSecondRow.isEmpty())
                size.setWidth(size.width() + 4 + 5);

            *strFirsRect = QRect(QPoint(0, rect.bottom() + 2), size);
            rect = rect.united(*strFirsRect);
        }
    }
    if (!strSecondRow.isEmpty())
    {
        QSize size = fMetric.size(Qt::TextSingleLine, strSecondRow);
        if (!size.isEmpty())
        {
            if (hasMenu)
                size.setWidth(size.width() + 4 + 5);
            *strSecondRect = QRect(QPoint(0, rect.bottom() + 2), size);
            rect = rect.united(*strSecondRect);
        }
    }

    int dx = (rc.width()-rect.width())/2;
    int dy = (rc.height()-rect.height())/2;

    if (!pmRect->isEmpty())
    {
        pmRect->translate(dx, dy);
        pmRect->translate((rect.width()-pmRect->width())/2, 0);
    }

    if (!strFirsRect->isEmpty())
    {
        strFirsRect->translate(dx, dy);
        strFirsRect->translate((rect.width()-strFirsRect->width())/2, 0);
    }

    if (!strSecondRect->isEmpty())
    {
        strSecondRect->translate(dx, dy);
        strSecondRect->translate((rect.width()-strSecondRect->width())/2, 0);
    }
}

bool RibbonStyle::drawToolButtonLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const RibbonBackstageButton*>(w))
    {
        if (const StyleOptionBackstageButton* toolbutton = qstyleoption_cast<const StyleOptionBackstageButton*>(opt))
        {
            QRect rect = toolbutton->rect;
            QPalette pal = toolbutton->state & State_Enabled ? QPalette() : toolbutton->palette;

            // Arrow type always overrules and is always shown
            bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
            if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty())|| toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) 
            {
                int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
                if (!styleHint(SH_UnderlineShortcut, opt, w))
                    alignment |= Qt::TextHideMnemonic;

                proxy()->drawItemText(p, rect, alignment, /*toolbutton->palette*/pal, /*opt->state & State_Enabled*/true, 
                    toolbutton->text, QPalette::ButtonText);
            }
            else
            {
                QPixmap pm;
                QSize pmSize = toolbutton->iconSize;
                if (pmSize.width() > qMin(rect.width(), rect.height()))
                {
                    const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                    pmSize = QSize(iconExtent, iconExtent);
                }

                if (!toolbutton->icon.isNull())
                {
                    QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                    QIcon::Mode mode;
                    if (!(toolbutton->state & State_Enabled))
                        mode = QIcon::Disabled;
                    else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                        mode = QIcon::Active;
                    else
                        mode = QIcon::Normal;
                    pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
                    pmSize = pm.size();
                }

                if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
                {
                    int alignment = Qt::TextShowMnemonic;
                    if (!proxy()->styleHint(SH_UnderlineShortcut, opt, w))
                        alignment |= Qt::TextHideMnemonic;

                    p->setFont(toolbutton->font);

                    if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                    {
                        QRect pmRect, strFirsRect, strSecondRect;
                        calcRects(rect, pmSize, toolbutton->fontMetrics, toolbutton->text, 
                            toolbutton->features & QStyleOptionToolButton::HasMenu, &pmRect, &strFirsRect, &strSecondRect);
                    
                        if (!hasArrow)
                            drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pmRect), Qt::AlignHCenter, pm);
    //                    else 
    //                        drawArrow(this, toolbutton, pr, p, w);

                        alignment |= Qt::AlignCenter;

                        QString strFirstRow, strSecondRow;
                        splitString(toolbutton->text, strFirstRow, strSecondRow);

                        if (!strFirstRow.isEmpty())
                        {
                            if (toolbutton->features & QStyleOptionToolButton::HasMenu && strSecondRow.isEmpty())
                            {
                                alignment &= ~Qt::AlignCenter;
                                alignment |= Qt::AlignVCenter | Qt::AlignLeft;
                            }
                            proxy()->drawItemText(p, strFirsRect, alignment, /*toolbutton->palette*/pal,
                                /*toolbutton->state & State_Enabled*/true, strFirstRow, QPalette::ButtonText);
                        }

                        if (!strSecondRow.isEmpty()) 
                        {
                            if (toolbutton->features & QStyleOptionToolButton::HasMenu)
                            {
                                alignment &= ~Qt::AlignCenter;
                                alignment |= Qt::AlignVCenter | Qt::AlignLeft;
                            }
                            proxy()->drawItemText(p, strSecondRect, alignment, /*toolbutton->palette*/pal,
                                /*toolbutton->state & State_Enabled*/true, strSecondRow, QPalette::ButtonText);
                        }
                    }
                    else
                    {
                        QRect pmRect(QPoint(4, (rect.height() - pmSize.height())/2), pmSize);
                        if (!hasArrow) 
                            drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pmRect), Qt::AlignCenter, pm);
//                        else 
//                            drawArrow(this, toolbutton, pr, p, w);
                        alignment |= Qt::AlignLeft | Qt::AlignVCenter;

                        QSize sizeText = toolbutton->fontMetrics.size(Qt::TextSingleLine, toolbutton->text);
                        QRect tcText(QPoint(pmRect.right() + 4, (rect.height() - sizeText.height())/2), sizeText);
                        proxy()->drawItemText(p, tcText, alignment, /*toolbutton->palette*/pal,
                            /*toolbutton->state & State_Enabled*/true, toolbutton->text, QPalette::ButtonText);
                    }
                }
                else
                {
//                    if (hasArrow) 
//                        drawArrow(this, toolbutton, rect, p, w);
//                    else 
                    {
                        QRect pr = rect;
                        if ((toolbutton->subControls & SC_ToolButtonMenu) || (toolbutton->features & QStyleOptionToolButton::HasMenu))
                        {
                            int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, opt, w);
                            pr.setWidth(pr.width()-mbi);
                        }
                        drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                    }
                }
            }
        }
        return true;
    }
    else
    {
        return OfficeStyle::drawToolButtonLabel(opt, p, w);
    }
}

bool RibbonStyle::drawToolButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const RibbonBackstageButton*>(w))
    {
        if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
        {
            QRect button, menuarea;
            button = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButton, w);
            menuarea = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButtonMenu, w);

            State flags = toolbutton->state & ~State_Sunken;

            if (flags & State_AutoRaise) 
            {
                if (!(flags & State_MouseOver) || !(flags & State_Enabled)) 
                    flags &= ~State_Raised;
            }
            State mflags = flags;
            if (toolbutton->state & State_Sunken) 
            {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    flags |= State_Sunken;
                mflags |= State_Sunken;
            }

            RibbonStyle* rs = (RibbonStyle*)(this);
            bool animation = isAnimationEnabled();
            rs->enableAnimation(false);
            proxy()->drawPrimitive(PE_PanelButtonTool, toolbutton, p, w);
            rs->enableAnimation(animation);

            QStyleOptionToolButton label = *toolbutton;
            label.state = flags;
            proxy()->drawControl(CE_ToolButtonLabel, &label, p, w);

            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (pmSize.width() > qMin(toolbutton->rect.width(), toolbutton->rect.height()))
            {
                const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                pmSize = QSize(iconExtent, iconExtent);
            }

            if (!toolbutton->icon.isNull())
            {
                QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(toolbutton->state & State_Enabled))
                    mode = QIcon::Disabled;
                else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;
                pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
                pmSize = pm.size();
            }

            if (toolbutton->subControls & SC_ToolButtonMenu) 
            {
                QStyleOption tool(0);
                tool.palette = toolbutton->palette;

                QRect ir = menuarea, rcArrow;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    QString strFirstRow, strSecondRow;
                    splitString(toolbutton->text, strFirstRow, strSecondRow);
                    rcArrow = QRect(QPoint(strSecondRow.isEmpty() ? opt->rect.width()/2 - 2 : opt->rect.right()-7, opt->rect.bottom()-8), QSize(5, 4));
                }
                else
                    rcArrow = QRect(QPoint((ir.left() + ir.right()-6) / 2, (ir.top() + ir.bottom()-5) / 2), QSize(5, 4));

                tool.rect = rcArrow;
                tool.state = mflags;

                proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p, w);
            } 
            else if (toolbutton->features & QStyleOptionToolButton::HasMenu) 
            {
                QStyleOptionToolButton newBtn = *toolbutton;
                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    QRect pmRect, strFirsRect, strSecondRect;
                    calcRects(toolbutton->rect, pmSize, toolbutton->fontMetrics, toolbutton->text, 
                        true, &pmRect, &strFirsRect, &strSecondRect);

                    QRect r = strSecondRect.isEmpty() ? strFirsRect : strSecondRect;
                    newBtn.rect = QRect(QPoint(r.right() - 5, r.top() + (r.height())/2), QSize(5, 5));
                }
                else
                {
                    QRect ir = menuarea;
                    int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, w);
                    newBtn.rect = QRect(QPoint(ir.right() + 4 - mbi, ir.y() + ir.height() - mbi), QSize(mbi - 5, mbi - 5));
                }
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, w);
            }
        }
        return true;
    }
    else
    {
        return OfficeStyle::drawToolButton(opt, p, w);
    }
}


bool RibbonStyle::drawToolBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolBar* toolbar = qstyleoption_cast<const QStyleOptionToolBar*>(opt)) 
    {
        QPixmap soImage = toolbar->state & QStyle::State_Horizontal ? cached("ToolbarFaceHorizontal.png")
        : cached("ToolbarFaceVertical.png");
        drawImage(soImage, *p, opt->rect, soImage.rect(), QRect(QPoint(5, 5), QPoint(5, 5)));
        return true;
    }
    return false;
}

bool RibbonStyle::drawGroupControlEntry(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if ( widget && /*rg->isControlsGrouping()*/widget->property(qtn_Prop_Group).toBool())
    {
        if (const QStyleOptionToolButton*toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
        {
            if (toolbutton->subControls & SC_ToolButtonMenu)
                return false;
        }

        const RibbonGroup * rg = qobject_cast<const RibbonGroup*>(widget ? widget->parentWidget() : Q_NULL);
        if (!rg)
            return false;

        bool beginGroup = widget->property(qtn_Prop_Begin_Group).toBool();
        bool wrap       = widget->property(qtn_Prop_Wrap).toBool();
        bool left       = beginGroup || wrap;

        int indexNext = rg->getNextIndex(widget);
        QWidget* nextWidget = rg->getWidget(indexNext);
        bool beginGroupNext = nextWidget->property(qtn_Prop_Begin_Group).toBool();
        bool wrapNext       = nextWidget->property(qtn_Prop_Wrap).toBool();

        bool right = indexNext <= rg->getIndexWidget(widget)|| beginGroupNext || wrapNext;

        QPixmap soImage = left && right  ? cached("ToolbarButtonsSpecialSingle.png") :
                                   left  ? cached("ToolbarButtonsSpecialLeft.png")   :
                                   right ? cached("ToolbarButtonsSpecialRight.png")  :
                                           cached("ToolbarButtonsSpecialCenter.png");
        
        if(soImage.isNull())
            return false;

        bool enabled  = opt->state & State_Enabled;
        bool checked  = opt->state & State_On;
        bool selected = opt->state & State_MouseOver;
        bool pressed  = opt->state & State_Sunken;
        bool popuped  = false;
        if (const QStyleOptionToolButton *toolbutton= qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
            popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);

        int state = 0;
        if (!enabled)
        {
//          if (isKeyboardSelected(selected)) state = 1;
        }

        else if (popuped) state = 2 + 1;
        else if (checked && !selected && !pressed) state = 3;
        else if (checked && selected && !pressed) state = 4;
        else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) state = 2;
        else if (selected || pressed) state = 1;

        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), state, 5), 
            QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

bool RibbonStyle::drawIndicatorArrow(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const 
{ 
    if (getParentWidget<RibbonBar>(w) || qobject_cast<const RibbonBackstageButton*>(w))
    {
        switch(pe)
        {
            case PE_IndicatorArrowDown :
                if (qobject_cast<const QToolButton*>(w))
                {
                    if (/*but->menu() &&*/ !qobject_cast<const QTabBar*>(w->parentWidget()))
                    {
                        QPixmap soIndicator = cached("ToolbarButtonDropDownGlyph.png");
                        bool enabled = opt->state & State_Enabled;    
                        bool selected = opt->state & State_Selected;
                        QRect rcSrc = getSource(soIndicator.rect(), !enabled ? 3 : selected ? 1 : 0, 4);
                        QRect rcEntry(QPoint(opt->rect.topLeft()), rcSrc.size());
                        drawImage(soIndicator, *p, rcEntry, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                        return true;
                    }
                }
                break;
            default:
                break;
        }
    }
    return OfficeStyle::drawIndicatorArrow(pe, opt, p, w);
}

void RibbonStyle::drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, bool checked, bool popuped,
                                BarType barType, BarPosition barPos) const
{
    if (barType != TypePopup)
    {
        bool smallSize = rect.height() < 33;
        QPixmap pixmap = smallSize ? cached("ToolbarButtons22.png") : cached("ToolbarButtons50.png");
        Q_ASSERT(!pixmap.isNull());
        if (pixmap.isNull())
            return;

        int state = -1;

        if (!enabled)
        {
            //            if (isKeyboardSelected(selected)) state = 0;
        }
        else if (popuped) state = 2;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 3;
        else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) state = 1;
        else if (selected || pressed) state = 0;

        if (state != -1)
            drawImage(pixmap, *p, rect, getSource(pixmap.rect(), state, 4), QRect(QPoint(8, 8), QPoint(8, 8)));
    }
    else if (barPos == BarPopup && selected && barType == TypePopup)
    {
        bool smallSize = rect.height() < 33;
        QPixmap soButton = !enabled ? smallSize ? cached("MenuSelectedDisabledItem22.png") : cached("MenuSelectedDisabledItem54.png") :
            smallSize ? cached("MenuSelectedItem22.png") : cached("MenuSelectedItem54.png");
        QRect rcSrc = soButton.rect();
        drawImage(soButton, *p, rect, rcSrc, QRect(QPoint(4, 4), QPoint(4, 4)));
    }
    else
    {
        OfficeStyle::drawRectangle(p, rect, selected, pressed, enabled, checked, popuped, barType, barPos);
    }
}

void RibbonStyle::drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& pal, bool enabled,
                               const QString& text, QPalette::ColorRole textRole) const
{
    if (text.isEmpty())
        return;
    QPen savedPen;
    if (textRole != QPalette::NoRole) 
    {
        savedPen = painter->pen();
        painter->setPen(QPen(pal.brush(textRole), savedPen.widthF()));
    }
    if (!enabled) 
    {
        if (proxy()->styleHint(SH_DitherDisabledText)) 
        {
            QRect br;
            painter->drawText(rect, flags, text, &br);
            painter->fillRect(br, QBrush(painter->background().color(), Qt::Dense5Pattern));
            return;
        } 
        else if (proxy()->styleHint(SH_EtchDisabledText)) 
        {
            QPen pen = painter->pen();
            painter->setPen(pal.light().color());
        }
    }
    painter->drawText(rect, flags, text);
    if (textRole != QPalette::NoRole)
        painter->setPen(savedPen);
}

void RibbonStyle::drawPanelBackstageButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const StyleOptionBackstageButton* btnStyle = qstyleoption_cast<const StyleOptionBackstageButton*>(opt)) 
    {
        QRect rc = btnStyle->rect;
        bool isDefault = btnStyle->features & QStyleOptionButton::DefaultButton;
        bool enabled  = opt->state & State_Enabled;
        bool checked  = opt->state & State_On;
        bool selected = opt->state & State_MouseOver;
        bool pressed  = opt->state & State_Sunken;
        //            bool popuped  = false;

        if (btnStyle->flatStyle && !selected && !pressed && !checked)
            return;

        QPixmap soImage = CommonStyle::cached(getPathForBackstageImages() + "BackstageButton.png");
        int state = isDefault ? 4 : 0;

        if (!enabled) state = 3;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 1;
        else if ((selected && pressed)) state = 2;
        else if (selected || pressed) state = 1;

        if (state != -1)
            drawImage(soImage, *p, rc, getSource(soImage.rect(), state, 5), QRect(QPoint(4, 4), QPoint(4, 4)), QColor(0xFF, 0, 0xFF));

        if (checked && btnStyle->tabStyle)
        {
            QPixmap soImage = CommonStyle::cached(getPathForBackstageImages() + "BackstageButtonGlyph.png");
            if (!soImage.isNull())
            {
                QRect rcSrc = soImage.rect();
                int y = rc.center().y();
                QRect rc(QPoint(rc.right() - rcSrc.width(), y - rcSrc.height() / 2), QPoint(rc.right(), y - rcSrc.height() / 2 + rcSrc.height()));
                drawImage(soImage, *p, rc, rcSrc);
            }
        }

    }
}

void RibbonStyle::drawFillRect(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (qobject_cast<const QMenu*>(widget ? widget->parentWidget() : Q_NULL))
    {
        p->fillRect(opt->rect, QColor(246, 246, 246));
        return;
    }

    bool enabled  = opt->state & State_Enabled;
    bool selected = opt->state & State_MouseOver;
    p->fillRect(opt->rect, selected && enabled ? m_clrControlGallerySelected : m_clrControlGalleryNormal);
}

void RibbonStyle::drawRect(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget)
    QPen savePen = p->pen();
    p->setPen(m_clrControlGalleryBorder);
    p->drawRect(opt->rect);
    p->setPen(savePen);
}

void RibbonStyle::drawKeyTip(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    DrawHelpers::drawGradientFill(*p, opt->rect, m_clrTooltipLight, m_clrTooltipDark, true);

    QPixmap soImage = cached("TooltipFrame.png");
    if (!soImage.isNull())
        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), 0, 1), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
    else
    {
        const QPen oldPen = p->pen();
        p->setPen(m_clrTooltipBorder);
        p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        p->setPen(oldPen);
    }
}

void RibbonStyle::drawBackstage(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const StyleOptionRibbonBackstage* optBackstage = qstyleoption_cast<const StyleOptionRibbonBackstage*>(opt)) 
    {
        QPixmap pixmap = cached(tr("BackstageTopBorder.png"));
        if (!pixmap.isNull())
            p->drawPixmap(QRect(QPoint(0, 0), QSize(opt->rect.width(), 2)), pixmap, pixmap.rect());

        pixmap = cached(tr("BackstageMenuBackground.png"));
        if (!pixmap.isNull())
            p->drawPixmap(QRect(QPoint(0, 2), QPoint(optBackstage->menuWidth, opt->rect.bottom())), pixmap, pixmap.rect());
    }
}

template<class T>
static QWidget* getPrevParentWidget(QWidget* pWidget)
{
    if (qobject_cast<T*>(pWidget))
        return Q_NULL;

    QWidget* pPrevWidget = pWidget;
    while (pWidget)
    {
        pWidget = pWidget->parentWidget();
        if (qobject_cast<T*>(pWidget))
            return pPrevWidget;
        else
            pPrevWidget = pWidget;
    }
    return Q_NULL;
}

bool RibbonStyle::showToolTip(const QPoint& pos, QWidget* w)
{
    Q_UNUSED(pos);
    if ((qobject_cast<QToolButton*>(w) && getParentWidget<RibbonSystemPopupBar>(w)) ||
        qobject_cast<RibbonBackstageButton*>(w))
        return true;

    if (/*const RibbonBar* ribbonBar =*/ getParentWidget<RibbonBar>(w))
    {
        QPoint p = pos;
        p += QPoint(2,
        #ifdef Q_WS_WIN
            21
        #else
            16
        #endif
            );

        int posX = p.x();
        int posY = p.y();

        if (const RibbonGroup* group = getParentWidget<RibbonGroup>(w))
        {
            Q_UNUSED(group);
            posX = w->mapToGlobal(w->rect().topLeft()).x();

            if (QWidget* prevWidget = getPrevParentWidget<RibbonGroup>(w))
            {
                if (RibbonGallery* gallery = qobject_cast<RibbonGallery*>(w))
                {
                    QRect rect = gallery->getItemDrawRect(gallery->getSelectedItem());
                    posX = prevWidget->mapToGlobal(rect.topLeft()).x();
                }
                else
                    posX = prevWidget->mapToGlobal(prevWidget->rect().topLeft()).x();
            }
            if (const RibbonPage* page = getParentWidget<RibbonPage>(w))
                posY = page->mapToGlobal(page->rect().bottomRight()).y() + 2;
        }

        QIcon icon;
        QString strTitleText;
        QString strTipText = w->toolTip();

        if (RibbonGallery* gallery = qobject_cast<RibbonGallery*>(w))
        {
            if (RibbonGalleryItem* item = gallery->getItem(gallery->getSelectedItem()))
            {
//                icon = item->icon();
                QString str = item->toolTip();
                if (strTipText != str)
                    strTipText = str;
            }
        }
        else if (strTipText.isEmpty())
        {
            QWidget* widget = w->parentWidget();

            if (!qobject_cast<RibbonGroup*>(widget))
            {
                if (RibbonPageSystemRecentFileList* recentFile = qobject_cast<RibbonPageSystemRecentFileList*>(w))
                {
                    if (QAction* currentAction = recentFile->getCurrentAction())
                        strTipText = currentAction->data().toString();
                }
                else
                {
                    while (widget)
                    {
                        strTipText = widget->toolTip();
                        if (!strTipText.isEmpty())
                            break;
                        widget = widget->parentWidget();
                        if (qobject_cast<RibbonGroup*>(widget))
                            break;
                    }
                }
            }
        }
        else if (QToolButton* button = qobject_cast<QToolButton*>(w))
        {
            icon = button->icon();
            QString str = button->text();
            str.remove(QChar('&'));
            if (strTipText != str)
                strTitleText = str;
        }

        if (icon.isNull())
        {
            if (const ExWidgetWrapper* wrapper = getParentWidget<ExWidgetWrapper>(w))
                icon = wrapper->icon();
        }

        QPoint globalPos(posX, posY);
        RibbonToolTip::showToolTip(globalPos, strTitleText, strTipText, icon, w);
        return true;
    }
    return false;
}

QString RibbonStyle::getPathForBackstageImages() const
{
    QString pathCached = ":/res/Office2007Blue/";
    if (isStyle2010())
        pathCached = ":/res/Office2010Blue/";
    else if (getOptionStyle() == OS_WINDOWS7SCENIC)
        pathCached = ":/res/Windows7Scenic/";
    return pathCached;
}

static RibbonBar* findMainWindow()
{
    foreach (QWidget *widget, qApp->topLevelWidgets()) 
    {
        if (qobject_cast<QMainWindow *>(widget))
        {
            QList<RibbonBar*> l = qFindChildren<RibbonBar*>(widget);
            if (l.size() > 0)
                return l.at(0);
        }
    }
    return Q_NULL;
}

bool RibbonStyle::eventFilter(QObject* watched, QEvent* event)
{
    QTN_D(RibbonStyle);
    switch (event->type())
    {
        case QEvent::HoverMove :
        case QEvent::HoverEnter :
        case QEvent::HoverLeave :
            if (QTabBar* tabBar = qobject_cast<QTabBar*>(watched))
            {
                if (qobject_cast<QMdiArea*>(tabBar->parentWidget()))
                {
                    QTN_D(RibbonStyle);
                    d.tabHoverUpdate(tabBar, event);
                }
            }
            break;
        case QEvent::KeyPress:
            if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Alt) 
            {
                d.completeKey_ = static_cast<QKeyEvent*>(event)->text().isEmpty();
                if (QMainWindow* widget = qobject_cast<QMainWindow*>(watched))
                {
                    QList<RibbonBar*> l = qFindChildren<RibbonBar*>(widget);
                    for (int i = 0; i < l.size(); ++i)
                    {
                        bool hasKeyTips = l.at(i)->qtn_d().keyTips_.count() > 0;
                        HideKeyTipEvent ktEvent;
                        QApplication::sendEvent(l.at(i), &ktEvent);
                        d.destroyKeyTips_ = hasKeyTips;
                    }
                }
            }
            break;
        case QEvent::KeyRelease:
            if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Alt) 
            {
                if (QMainWindow* widget = qobject_cast<QMainWindow*>(watched))
                {
                    if (!d.destroyKeyTips_)
                    {
                        QList<RibbonBar*> l = qFindChildren<RibbonBar*>(widget);
                        for (int i = 0; i < l.size(); ++i)
                        {
                            if (!l.at(i)->isBackstageVisible())
                            {
                                ShowKeyTipEvent ktEvent(l.at(i));
                                QApplication::sendEvent(l.at(i), &ktEvent);
                                l.at(i)->setFocus();
                            }
                        }
                    }
                    else
                        d.destroyKeyTips_ = false;
                }
                d.completeKey_ = false;
            }
            break;
        case QEvent::Show :
            if (QMenu* menu = qobject_cast<QMenu*>(watched))
            {
                if (RibbonBar* ribbonBar = ::findMainWindow())
                {
                    if (ribbonBar->qtn_d().levels_.size() > 0 || (qobject_cast<RibbonSystemPopupBar*>(watched) && d.completeKey_))
                        QApplication::postEvent(ribbonBar, new ShowKeyTipEvent(menu));
                }
                d.completeKey_ = false;
            }
            break;
        case QEvent::FontChange :
            if (qobject_cast<RibbonBar*>(watched) || 
                qobject_cast<Qtitan::RibbonPage*>(watched) || 
                qobject_cast<Qtitan::RibbonGroup*>(watched) ||
                qobject_cast<Qtitan::RibbonTabBar*>(watched))
            {
                d.refreshMetrics((QWidget*)watched);
            }
            break;
        default:
            break;
    }
    return OfficeStyle::eventFilter(watched, event);
}

/* RibbonStylePlugin */
QStringList RibbonStylePlugin::keys() const
{
    return QStringList() << "RibbonStyle";
}

QStyle* RibbonStylePlugin::create( const QString& key )
{
    if ( key.toLower() == QLatin1String("ribbonstyle"))
        return new RibbonStyle();
    return Q_NULL;
}

QObject* qt_plugin_instance_ribbonstyle()
{
    static QObject* instance = Q_NULL;
    if ( !instance )
        instance = new RibbonStylePlugin();
    return instance;
}

Q_IMPORT_PLUGIN(ribbonstyle)
