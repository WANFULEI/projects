/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced OfficeFrameHelperWin for Qt)
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
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QPaintEngine>
#include <QStyleOption>
#include <QStatusBar>
#include <QToolButton>
#include <QLibrary>
#include <QDesktopWidget>
#include <qevent.h>

#include "QtnRibbonSystemPopupBar.h"
#include "QtnRibbonMainWindow.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonTabBar.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonStyle.h"
#include "QtnOfficeFrameHelper_win.h"

using namespace Qtitan;


//DWM related
typedef struct          //MARGINS       
{
    int cxLeftWidth;    // width of left border that retains its size
    int cxRightWidth;   // width of right border that retains its size
    int cyTopHeight;    // height of top border that retains its size
    int cyBottomHeight; // height of bottom border that retains its size
} HLP_MARGINS;

typedef struct //DTTOPTS
{
    DWORD dwSize;
    DWORD dwFlags;
    COLORREF crText;
    COLORREF crBorder;
    COLORREF crShadow;
    int eTextShadowType;
    POINT ptShadowOffset;
    int iBorderSize;
    int iFontPropId;
    int iColorPropId;
    int iStateId;
    BOOL fApplyOverlay;
    int iGlowSize;
} HLP_DTTOPTS;

typedef struct 
{
    DWORD dwFlags;
    DWORD dwMask;
} HLP_WTA_OPTIONS;

#define HLP_WM_THEMECHANGED                 0x031A
#define HLP_WM_DWMCOMPOSITIONCHANGED        0x031E

enum HLP_WINDOWTHEMEATTRIBUTETYPE 
{
    HLP_WTA_NONCLIENT = 1
};

#define HLP_WTNCA_NODRAWCAPTION 0x00000001
#define HLP_WTNCA_NODRAWICON    0x00000002

#define HLP_DT_CENTER           0x00000001 // DT_CENTER
#define HLP_DT_VCENTER          0x00000004
#define HLP_DT_SINGLELINE       0x00000020
#define HLP_DT_NOPREFIX         0x00000800

enum HLP_NAVIGATIONPARTS           // NAVIGATIONPARTS
{
    HLP_NAV_BACKBUTTON = 1,
    HLP_NAV_FORWARDBUTTON = 2,
    HLP_NAV_MENUBUTTON = 3,
};

enum HLP_NAV_BACKBUTTONSTATES      // NAV_BACKBUTTONSTATES
{
    HLP_NAV_BB_NORMAL = 1,
    HLP_NAV_BB_HOT = 2,
    HLP_NAV_BB_PRESSED = 3,
    HLP_NAV_BB_DISABLED = 4,
};

enum HLP_THEME_SIZE 
{
    TS_MIN,
    TS_TRUE,
    TS_DRAW
};


#define HLP_TMT_CAPTIONFONT (801)           //TMT_CAPTIONFONT
#define HLP_DTT_COMPOSITED  (1UL << 13)     //DTT_COMPOSITED
#define HLP_DTT_GLOWSIZE    (1UL << 11)     //DTT_GLOWSIZE

#define HLP_WM_NCMOUSELEAVE 674             //WM_NCMOUSELEAVE

#define HLP_WP_CAPTION             1 // WP_CAPTION
#define HLP_WP_MAXCAPTION          5 // WP_MAXCAPTION
#define HLP_CS_ACTIVE              1 // CS_ACTIVE
#define HLP_FS_ACTIVE              1
#define HLP_FS_INACTIVE            2
#define HLP_TMT_FILLCOLORHINT   3821 // TMT_FILLCOLORHINT
#define HLP_TMT_BORDERCOLORHINT 3822 // TMT_BORDERCOLORHINT


typedef BOOL (WINAPI *PtrDwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const HLP_MARGINS* pMarInset);
typedef HRESULT (WINAPI *PtrSetWindowThemeAttribute)(HWND hwnd, enum HLP_WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute);

static PtrDwmDefWindowProc pDwmDefWindowProc = 0;
static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled = 0;
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;
//static PtrSetWindowThemeAttribute pSetWindowThemeAttribute = 0;

//Theme related
typedef bool (WINAPI *PtrIsAppThemed)();
typedef bool (WINAPI *PtrIsThemeActive)();
typedef HANDLE (WINAPI *PtrOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (WINAPI *PtrCloseThemeData)(HANDLE hTheme);
typedef HRESULT (WINAPI *PtrGetThemeSysFont)(HANDLE hTheme, int iFontId, LOGFONTW *plf);
typedef HRESULT (WINAPI *PtrDrawThemeTextEx)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const HLP_DTTOPTS *pOptions);
typedef HRESULT (WINAPI *PtrDrawThemeBackground)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
typedef HRESULT (WINAPI *PtrGetThemePartSize)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, OPTIONAL RECT *prc, HLP_THEME_SIZE eSize, OUT SIZE *psz);
typedef HRESULT (WINAPI *PtrGetThemeColor)(HANDLE hTheme, int iPartId, int iStateId, int iPropId, OUT COLORREF *pColor);

enum WIS_UX_BUFFERFORMAT
{
    WIS_BPBF_COMPATIBLEBITMAP,    // Compatible bitmap
    WIS_BPBF_DIB,                 // Device-independent bitmap
    WIS_BPBF_TOPDOWNDIB,          // Top-down device-independent bitmap
    WIS_BPBF_TOPDOWNMONODIB       // Top-down monochrome device-independent bitmap
};

// BP_PAINTPARAMS
struct WIS_UX_PAINTPARAMS
{
    DWORD                       cbSize;
    DWORD                       dwFlags; // BPPF_ flags
    const RECT *                prcExclude;
    const BLENDFUNCTION *       pBlendFunction;
};

//typedef HANDLE (WINAPI *PtrBeginBufferedPaint)(HDC hdcTarget, const RECT* rect, WIS_UX_BUFFERFORMAT dwFormat, WIS_UX_PAINTPARAMS* pPaintParams, HDC* phdc);

static PtrIsAppThemed pIsAppThemed = 0;
//static PtrIsThemeActive pIsThemeActive = 0;
static PtrOpenThemeData pOpenThemeData = 0;
//static PtrCloseThemeData pCloseThemeData = 0;
static PtrGetThemeSysFont pGetThemeSysFont = 0;
static PtrDrawThemeTextEx pDrawThemeTextEx = 0;
//static PtrDrawThemeBackground pDrawThemeBackground = 0;
//static PtrGetThemePartSize pGetThemePartSize = 0;
//static PtrGetThemeColor pGetThemeColor = 0;

//static PtrBeginBufferedPaint pBeginBufferedPaint = 0;

/*!
\class Qtitan::OfficeFrameHelper
\internal
*/

/*!
\class Qtitan::OfficeFrameHelperWin
\internal
*/
bool OfficeFrameHelperWin::m_allowDwm = true;
OfficeFrameHelperWin::OfficeFrameHelperWin(QWidget* parent)
    : QObject(parent)
{
    m_hIcon = Q_NULL;
    m_frame = Q_NULL;
    m_hwndFrame = Q_NULL;
    m_listContextHeaders = Q_NULL;
    m_dwmEnabled = false;
    m_lockNCPaint = false;
    m_inUpdateFrame = false;
    m_inLayoutRequest = false;
    m_szFrameRegion = QSize();
    m_collapseTop = -1;
    m_collapseBottom = -1;
    m_frameBorder = 0;
    m_borderSizeBotton = 0;
    m_skipNCPaint = true;
    m_active = true;
    m_officeFrameEnabled = false;
    m_wasFullScreen = false;

    if (!resolveSymbols())
       Q_ASSERT(false);
}

OfficeFrameHelperWin::~OfficeFrameHelperWin()
{
    if (m_frame)
        m_frame->removeEventFilter(this);

    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
        mainWindow->setFrameHelper(Q_NULL);

    if (m_officeFrameEnabled)
        enableOfficeFrame(Q_NULL);

    resetIcon();
}

void OfficeFrameHelperWin::enableOfficeFrame(QWidget* widget)
{
    bool enabled = widget != Q_NULL;
    if (m_officeFrameEnabled == enabled)
        return;

    m_ribbonBar = qobject_cast<RibbonBar*>(parent());
    m_frame = widget;
    m_active = true;
    m_officeFrameEnabled = enabled;

    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
        mainWindow->setFrameHelper(this);

    if (m_frame)
    {
        m_hwndFrame = m_frame->winId();
        m_dwmEnabled = !m_allowDwm || getStyle() & WS_CHILD ? false : isCompositionEnabled();

        m_frame->installEventFilter(this);

        if (!m_dwmEnabled)
            refreshFrameStyle();
        else
            SetWindowRgn(m_hwndFrame, NULL, true);

        updateFrameRegion();
        ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    else
    {
        disableOfficeFrame();
    }
}

void OfficeFrameHelperWin::disableOfficeFrame()
{
    if (m_frame)
        m_frame->removeEventFilter(this);

    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
        mainWindow->setFrameHelper(Q_NULL);

    m_frame = NULL;
    m_ribbonBar = NULL;
    m_officeFrameEnabled = false;

    if (m_hwndFrame)
    {
        if (m_dwmEnabled)
        {
            HLP_MARGINS margins;
            margins.cxLeftWidth = 0;
            margins.cyTopHeight = 0;
            margins.cxRightWidth = 0;
            margins.cyBottomHeight = 0;
            pDwmExtendFrameIntoClientArea(m_hwndFrame, &margins);
        }

        SetWindowRgn(m_hwndFrame, (HRGN)NULL, true);
        ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    m_hwndFrame = NULL;
    m_dwmEnabled = false;
}

bool OfficeFrameHelperWin::isDwmEnabled() const
{
    return m_dwmEnabled;
}

bool OfficeFrameHelperWin::isActive() const
{
    return m_active;
}

int OfficeFrameHelperWin::getFrameBorder() const
{
    return m_frameBorder;
}

DWORD OfficeFrameHelperWin::getStyle(bool exStyle) const
{
    return m_hwndFrame ? (DWORD)GetWindowLongW(m_hwndFrame, exStyle ? GWL_EXSTYLE : GWL_STYLE) : 0;
}

int OfficeFrameHelperWin::titleBarSize() const
{ 
    if (isTitleVisible() || (!m_ribbonBar && m_frame))
        return m_frame->style()->pixelMetric(QStyle::PM_TitleBarHeight, 0) + m_frameBorder;

    if (m_ribbonBar)
        return m_ribbonBar->titleBarHeight() + m_frameBorder;

    return frameSize() + captionSize(); 
}

int OfficeFrameHelperWin::tabBarHeight() const
{
    if (m_ribbonBar)
        return m_ribbonBar->tabBarHeight();
    return 0;
}

void OfficeFrameHelperWin::setHeaderRect(const QRect& rcHeader)
{
    m_rcHeader = rcHeader;
}

void OfficeFrameHelperWin::setContextHeader(QList<ContextHeader*>* listContextHeaders)
{
    m_listContextHeaders = listContextHeaders;
}

QPixmap OfficeFrameHelperWin::getFrameSmallIcon() const
{
    if (!m_hwndFrame)
        return QPixmap();

    DWORD dwStyle = getStyle();
    if ((dwStyle & WS_SYSMENU) == 0)
        return QPixmap();

    HICON hIcon = (HICON)(DWORD_PTR)::SendMessageW(m_hwndFrame, WM_GETICON, ICON_SMALL, 0);
    if (!hIcon)
        hIcon = (HICON)(DWORD_PTR)::GetClassLongPtrW(m_hwndFrame, GCLP_HICONSM);

    return QPixmap::fromWinHICON(hIcon);
}

QString OfficeFrameHelperWin::getSystemMenuString(uint item) const
{
    Q_ASSERT(m_hwndFrame != Q_NULL);
    UINT id = SC_CLOSE;
    switch (item)
    {
        case QStyle::SC_TitleBarCloseButton :
                id = SC_CLOSE;
            break;
        case QStyle::SC_TitleBarNormalButton :
                id = SC_RESTORE;
            break;
        case QStyle::SC_TitleBarMaxButton :
               id = SC_MAXIMIZE;
            break;
        case QStyle::SC_TitleBarMinButton :
               id = SC_MINIMIZE;
           break;
        default :
           break;
    }

    HMENU hMenu = ::GetSystemMenu(m_hwndFrame, 0);
    Q_ASSERT(::IsMenu(hMenu)); 
    if (hMenu)
    {
        // offer no buffer first
        int len = ::GetMenuStringW(hMenu, id, NULL, 0, MF_BYCOMMAND);

        // use exact buffer length
        if (len > 0)
        {
            ushort* lpBuf = new ushort[len + 1];
            ::GetMenuStringW(hMenu, id, (LPTSTR)lpBuf, len+1, MF_BYCOMMAND);

            QString str = QString::fromUtf16(lpBuf, len);
            int index = str.indexOf(QLatin1String("\t"));
            if (index> 0)
                str = str.left(index);

            delete [] lpBuf;
            return str;
         }
    }
    return QString();
}

void OfficeFrameHelperWin::refreshFrameStyle()
{
    if (m_dwmEnabled)
        return;

    DWORD dwStyle = getStyle();
    DWORD dwStyleRemove = (WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL);

    if (dwStyle & dwStyleRemove)
    {
        m_lockNCPaint = true;
        SetWindowLongW(m_hwndFrame, GWL_STYLE, dwStyle & ~dwStyleRemove);

        RECT rc;
        ::GetWindowRect(m_hwndFrame, &rc);
        ::SendMessageW(m_hwndFrame, WM_NCCALCSIZE, false, (LPARAM)&rc);

        SetWindowLongW(m_hwndFrame, GWL_STYLE, dwStyle);

        m_lockNCPaint = false;
        redrawFrame();
    }
}

void OfficeFrameHelperWin::updateFrameRegion()
{
    if (!::IsWindow(m_hwndFrame))
        return;

    RECT rc;
    ::GetWindowRect(m_hwndFrame, &rc);
    updateFrameRegion(QSize(rc.right - rc.left, rc.bottom - rc.top), true);
}

void OfficeFrameHelperWin::updateFrameRegion(const QSize& szFrameRegion, bool bUpdate)
{
    if (m_inUpdateFrame)
        return;

    m_inUpdateFrame = true;

    if (m_szFrameRegion != szFrameRegion || bUpdate)
    {
        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = szFrameRegion.width();
        rc.bottom = szFrameRegion.height();
        AdjustWindowRectEx(&rc, getStyle(false), false, getStyle(true));

        m_frameBorder = -rc.left;
        m_borderSizeBotton = m_frameBorder;

        if (!m_dwmEnabled)
        {
            HRGN hRgn = 0;

            if (!isMDIMaximized())
            {
                if (getStyle() & WS_MAXIMIZE)
                {
                    int nFrameRegion = m_frameBorder - (getStyle(true) & WS_EX_CLIENTEDGE ? 2 : 0);
                    hRgn = CreateRectRgn(nFrameRegion, nFrameRegion, szFrameRegion.width() - nFrameRegion, szFrameRegion.height() - nFrameRegion);
                }
                else
                    hRgn = calcFrameRegion(szFrameRegion);
            }
            ::SetWindowRgn(m_hwndFrame, hRgn, true);
        }
        else
        {
            if (!(getStyle() & WS_MAXIMIZE))
                m_frameBorder = 4;

            bool flatFrame = (bool)m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_FlatFrame);

            int cyTopHeight = isTitleVisible() ? 0 : titleBarSize() + (flatFrame ? 0 : 1);

            if (m_ribbonBar && !isTitleVisible() && flatFrame)
                cyTopHeight += m_ribbonBar->tabBarHeight() + 1;

            HLP_MARGINS margins;
            margins.cxLeftWidth = 0;
            margins.cyTopHeight = cyTopHeight;
            margins.cxRightWidth = 0;
            margins.cyBottomHeight = 0;
            pDwmExtendFrameIntoClientArea(m_hwndFrame, &margins);

        }

        m_szFrameRegion = szFrameRegion;
    }

    m_inUpdateFrame = false;
}

bool OfficeFrameHelperWin::isMDIMaximized() const
{
    return (getStyle() & WS_MAXIMIZE) && (getStyle(TRUE) & WS_EX_MDICHILD);
}

bool OfficeFrameHelperWin::isTitleVisible() const
{
    if (!m_ribbonBar)
        return true;

    if (!m_ribbonBar->isVisible())
        return true;

    if ((getStyle() & (WS_CHILD | WS_MINIMIZE)) == (WS_CHILD | WS_MINIMIZE))
        return true;

    return false;
}

void OfficeFrameHelperWin::initStyleOption(StyleOptionFrame* option)
{
    RECT rc;
    ::GetWindowRect(m_hwndFrame, &rc);
    RECT rcClient;
    ::GetClientRect(m_hwndFrame, &rcClient);
    ::ClientToScreen(m_hwndFrame, (LPPOINT)&rcClient);
    ::ClientToScreen(m_hwndFrame, ((LPPOINT)&rcClient)+1);

    option->init(m_frame);
    option->rect = QRect(QPoint(rc.left, rc.top), QPoint(rc.right, rc.bottom)); 
    option->active = m_active;
    option->hasStatusBar = isFrameHasStatusBar();
    option->frameBorder = m_frameBorder;
    option->titleBarSize = titleBarSize();
    isFrameHasStatusBar(&option->statusHeight);
    option->titleVisible = isTitleVisible();
    option->clientRect = QRect(QPoint(rcClient.left, rcClient.top), QPoint(rcClient.right, rcClient.bottom));
    option->maximizeFlags = getStyle() & WS_MAXIMIZE;
}

static void drawPartFrame(QWidget* frame, HDC hdc, HDC cdc, const StyleOptionFrame& opt, const QRect& rect)
{
    QPixmap pixmap(rect.size());
    QPainter p(&pixmap);
    p.setWindow(rect);

    frame->style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, frame);
    if (HBITMAP newBmp = pixmap.toWinHBITMAP())
    {
        HBITMAP oldb = (HBITMAP)SelectObject(cdc, newBmp);
        ::BitBlt(hdc, rect.left(), rect.top(), rect.width(), rect.height(), cdc, 0, 0, SRCCOPY);
        ::SelectObject(cdc, oldb);
        ::DeleteObject(newBmp);
    }
}

void OfficeFrameHelperWin::redrawFrame()
{
    if (!m_dwmEnabled)
    {
        RECT rc;
        ::GetWindowRect(m_hwndFrame, &rc);

        int height = rc.bottom - rc.top;
        int width = rc.right - rc.left;

        HDC hdc = ::GetWindowDC(m_hwndFrame);
        HDC cdc = CreateCompatibleDC(hdc);

        StyleOptionFrame opt;
        initStyleOption(&opt);
        // draw top side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, 0, width, opt.titleVisible ? opt.titleBarSize : m_frameBorder));
        // draw right side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(width - m_frameBorder, 0, m_frameBorder, height));
        // draw left side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, 0, m_frameBorder, height));
        // draw bottom side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, height - m_borderSizeBotton, width, m_borderSizeBotton));
        ::DeleteDC(cdc);

        if (opt.titleVisible)
        {
            QRect rect(QPoint(rc.left, rc.top), QPoint(rc.right, rc.bottom));

            int nFrameBorder = getFrameBorder();
            int nTopBorder = isDwmEnabled() ? nFrameBorder : 0;

            if (HICON hIcon = getFrameSmallIcon().toWinHICON())
            {
                int nFrameBorder = getFrameBorder();

                QSize szIcon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));

                int nTop = nFrameBorder;
                int nLeft = nFrameBorder + 1;

                QRect rect(QPoint(nLeft, nTop), szIcon);
                ::DrawIconEx(hdc, rect.left(), rect.top(), hIcon, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT);
                ::DestroyIcon(hIcon);
            }

            ::SetBkMode(hdc, TRANSPARENT);

            HFONT hCaptionFont = getCaptionFont(Q_NULL);
            HFONT hOldFont = (HFONT)SelectObject(hdc, (HGDIOBJ) hCaptionFont);

            QRect rctext = rect;
            rctext.setHeight(opt.titleBarSize);

            QRgb clrFrameCaptionText = 0;
            if (!opt.active)
            {
                Qtitan::StyleHintReturnThemeColor hintReturnInActive(tr("Window"), tr("CaptionTextInActive"));
                clrFrameCaptionText = static_cast<QRgb>(m_frame->style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturnInActive));
            }
            else
            {
                Qtitan::StyleHintReturnThemeColor hintReturnActive(tr("Window"), tr("CaptionTextActive"));
                clrFrameCaptionText = static_cast<QRgb>(m_frame->style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturnActive));
            }

            COLORREF colorInActive = RGB(qRed(clrFrameCaptionText), qGreen(clrFrameCaptionText), qBlue(clrFrameCaptionText));
            ::SetTextColor(hdc, colorInActive);

            int len = ::GetWindowTextLengthW(m_hwndFrame);
            wchar_t* text = new wchar_t[++len];

            ::GetWindowTextW(m_hwndFrame, text, len + 1);

            QSize szIcon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
            RECT rc = {nFrameBorder + szIcon.width() + 3, nTopBorder, opt.rect.width() - nFrameBorder /*-100*/, opt.titleBarSize};

            UINT nFormat = DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
            ::DrawTextW(hdc, text, len, &rc, nFormat);
            delete[] text;
            SelectObject(hdc, (HGDIOBJ) hOldFont);
        }

        ::ReleaseDC(m_hwndFrame, hdc);
    }
}

void OfficeFrameHelperWin::drawTitleBar(QPainter* painter, const StyleOptionTitleBar& opt)
{
    if (!m_ribbonBar)
        return;

    HDC hdc = painter->paintEngine()->getDC();

    QRect rc = opt.rect;
    if ((bool)m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_FlatFrame))
        rc.adjust(0,0,0,1);

    drawBlackRect(hdc, rc);
//    fillSolidRect(painter, rc, QColor(0, 0, 0));

    int len = ::GetWindowTextLengthW(m_hwndFrame);
    wchar_t* text = new wchar_t[++len];
    ::GetWindowTextW(m_hwndFrame, text, len + 1);
    drawDwmCaptionText(painter, opt.rcTitleText, text[0] != Q_NULL ? QString::fromWCharArray(text) : opt.text, opt.state & QStyle::State_Active);
    delete[] text;

    if (opt.drawIcon && !opt.icon.isNull()) 
    {
        int nFrameBorder = getFrameBorder();
        int nTopBorder = isDwmEnabled() ? nFrameBorder : 0;

        QSize szIcon = getSizeSystemIcon();

        int nTop = nTopBorder / 2 - 1 + (opt.rect.bottom()  - szIcon.height())/2;
        int nLeft = opt.rect.left() + nFrameBorder + 1;

        bool destroy = false; 
        QRect rect(QPoint(nLeft, nTop), szIcon);

        //m_hIcon = (HICON)(DWORD_PTR)::GetClassLongPtr(m_hwndFrame, GCLP_HICONSM);

        if (!m_hIcon && !opt.icon.isNull())
        {
            m_hIcon = opt.icon.pixmap(szIcon).toWinHICON();
            destroy = true;
        }

        if (m_hIcon)
            ::DrawIconEx(hdc, rect.left(), rect.top(), m_hIcon, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT);

//        if (destroy)
//            ::DestroyIcon(m_hIcon);
    }

    if (hdc)
        painter->paintEngine()->releaseDC(hdc);
}

void OfficeFrameHelperWin::fillSolidRect(QPainter* painter, const QRect& rect, QColor clr)
{
    HDC hdc = painter->paintEngine()->getDC();

    RECT rc;
    rc.left    = rect.left();
    rc.top     = rect.top();
    rc.right   = rect.right();
    rc.bottom  = rect.bottom();
    ::SetBkColor(hdc, RGB(clr.red(), clr.green(), clr.blue()));
    ::ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

    if (hdc)
        painter->paintEngine()->releaseDC(hdc);
}

void OfficeFrameHelperWin::drawBlackRect(HDC hdc, const QRect& rect)
{
    // Set up a memory DC and bitmap that we'll draw into
    HDC dcMem;
    HBITMAP bmp;
    BITMAPINFO dib = {{0}};
    dcMem = CreateCompatibleDC(hdc);

    dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dib.bmiHeader.biWidth = rect.width();
    dib.bmiHeader.biHeight = -rect.height();
    dib.bmiHeader.biPlanes = 1;
    dib.bmiHeader.biBitCount = 32;
    dib.bmiHeader.biCompression = BI_RGB;

    bmp = CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(dcMem, (HGDIOBJ) bmp);

    BitBlt(hdc, rect.left(), rect.top(), rect.width(), rect.height(), dcMem, 0, 0, SRCCOPY);
    SelectObject(dcMem, (HGDIOBJ) hOldBmp);

    DeleteObject(bmp);
    DeleteDC(dcMem);
}

void OfficeFrameHelperWin::drawDwmCaptionText(QPainter* painter, const QRect& rect, const QString& strWindowText, bool active)
{
    if (!rect.isValid())
        return;

    HANDLE hTheme = pOpenThemeData(QApplication::desktop()->winId(), L"WINDOW");
    if (!hTheme) 
       return;

    // Set up a memory DC and bitmap that we'll draw into
    HDC dcMem;
    HBITMAP bmp;
    BITMAPINFO dib = {{0}};
    dcMem = CreateCompatibleDC(/*hdc*/0);

    dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dib.bmiHeader.biWidth = rect.width();
    dib.bmiHeader.biHeight = -rect.height();
    dib.bmiHeader.biPlanes = 1;
    dib.bmiHeader.biBitCount = 32;
    dib.bmiHeader.biCompression = BI_RGB;

    bmp = CreateDIBSection(/*hdc*/0, &dib, DIB_RGB_COLORS, NULL, NULL, 0);

    // Set up the DC
    HFONT hCaptionFont = getCaptionFont(hTheme);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(dcMem, (HGDIOBJ) bmp);
    HFONT hOldFont = (HFONT)SelectObject(dcMem, (HGDIOBJ) hCaptionFont);

    // Draw the text!
    HLP_DTTOPTS dto = { sizeof(HLP_DTTOPTS) };
    const UINT uFormat = DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
    RECT rctext = {0, 0, rect.width(), rect.height()};

    dto.dwFlags = HLP_DTT_COMPOSITED | HLP_DTT_GLOWSIZE;
    dto.iGlowSize = 8;//glowSize();

    pDrawThemeTextEx(hTheme, dcMem, getStyle() & WS_MAXIMIZE ? HLP_WP_MAXCAPTION : HLP_WP_CAPTION, active ? HLP_FS_ACTIVE : HLP_FS_INACTIVE, 
       (LPCWSTR)strWindowText.utf16(), -1, uFormat, &rctext, &dto);

    QPixmap pxx = QPixmap::fromWinHBITMAP(bmp, QPixmap::PremultipliedAlpha);
    painter->drawPixmap(rect.left(), rect.top(), pxx);

//  ::BitBlt(hdc, rect.left(), rect.top(), rect.width(), rect.height(), dcMem, 0, 0, SRCCOPY);
    SelectObject(dcMem, (HGDIOBJ) hOldBmp);
    SelectObject(dcMem, (HGDIOBJ) hOldFont);
    DeleteObject(bmp);
    DeleteObject(hCaptionFont);
    DeleteDC(dcMem);
}

QSize OfficeFrameHelperWin::getSizeSystemIcon() const
{
    return QSize(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
}

void OfficeFrameHelperWin::resetIcon()
{
    if (m_hIcon)
        ::DestroyIcon(m_hIcon);
    m_hIcon = Q_NULL;
}

static bool _qtnModifyStyle(HWND hWnd, int nStyleOffset, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
    Q_ASSERT(hWnd != NULL);
    DWORD dwStyle = ::GetWindowLongW(hWnd, nStyleOffset);
    DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
    if (dwStyle == dwNewStyle)
        return false;

    ::SetWindowLongW(hWnd, nStyleOffset, dwNewStyle);
    if (nFlags != 0)
        ::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
    return true;
}

bool OfficeFrameHelperWin::modifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
    return _qtnModifyStyle(hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags);
}

bool OfficeFrameHelperWin::isFrameHasStatusBar(int* statusHeight) const
{
    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
    {
        if (QWidget* statusBar = qFindChild<QStatusBar*>(mainWindow))
        {
            if (statusHeight)
                *statusHeight = statusBar->height();
            return statusBar->isVisible();
        }
    }
    return false;
}

HRGN OfficeFrameHelperWin::calcFrameRegion(QSize sz)
{
    if (getStyle() & WS_MAXIMIZE)
        return Q_NULL;

    int cx = sz.width(), cy = sz.height();

    RECT rgnTopFrame[] =
    {
        {4, 0, cx - 4, 1}, {2, 1, cx - 2, 2}, {1, 2, cx - 1, 3}, {1, 3, cx - 1, 4}, {0, 4, cx, cy - 4}
    };

    RECT rgnRibbonBottomFrame[] =
    {
        {1, cy - 4, cx - 1, cy - 2}, {2, cy - 2, cx - 2, cy - 1}, {4, cy - 1, cx - 4, cy - 0}
    };

    RECT rgnSimpleBottomFrame[] =
    {
        {0, cy - 4, cx, cy}
    };

    bool roundedCornersAlways = true;

    bool bHasStatusBar = (roundedCornersAlways || isFrameHasStatusBar()) && m_frameBorder > 3;

    int nSizeTopRect = sizeof(rgnTopFrame);
    int nSizeBottomRect = bHasStatusBar ? sizeof(rgnRibbonBottomFrame) : sizeof(rgnSimpleBottomFrame);

    int nSizeData = sizeof(RGNDATAHEADER) + nSizeTopRect + nSizeBottomRect;

    RGNDATA* pRgnData = (RGNDATA*)malloc(nSizeData);
    if (!pRgnData)
        return Q_NULL;

    memcpy(&pRgnData->Buffer, (void*)&rgnTopFrame, nSizeTopRect);
    memcpy(&pRgnData->Buffer + nSizeTopRect, bHasStatusBar ? (void*)&rgnRibbonBottomFrame : (void*)&rgnSimpleBottomFrame, nSizeBottomRect);

    pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
    pRgnData->rdh.iType = RDH_RECTANGLES;
    pRgnData->rdh.nCount = (nSizeTopRect + nSizeBottomRect) / sizeof(RECT);
    pRgnData->rdh.nRgnSize = 0;
    pRgnData->rdh.rcBound.left = 0;
    pRgnData->rdh.rcBound.top = 0;
    pRgnData->rdh.rcBound.right = cx;
    pRgnData->rdh.rcBound.bottom = cy;

    HRGN rgh = ::ExtCreateRegion(Q_NULL, nSizeData, pRgnData);

    free(pRgnData);
    return rgh;
}

HFONT OfficeFrameHelperWin::getCaptionFont(HANDLE hTheme)
{
    LOGFONTW lf = {0};

    if (!hTheme)
       pGetThemeSysFont(hTheme, HLP_TMT_CAPTIONFONT, &lf);
    else
    {
       NONCLIENTMETRICS ncm = {sizeof(NONCLIENTMETRICS)};
       SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false);
       lf = ncm.lfMessageFont;
    }
    return CreateFontIndirectW(&lf);
}

bool OfficeFrameHelperWin::hitTestContextHeaders(const QPoint& point) const
{
    if (!m_listContextHeaders)
        return false;

    for ( int i = 0, count = m_listContextHeaders->count(); i < count; i++)
    {
        ContextHeader* header =  m_listContextHeaders->at(i);
        if (header->rcRect.contains(point))
           return true;
    }
    return false;
}

#define qtn_GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define qtn_GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

bool OfficeFrameHelperWin::collapseTopFrameStrut()
{
    bool result = false;
    QWidgetData* data = qt_qwidget_data(m_frame);

    QRect qrect = data->crect;
    int collapseTop = qrect.y() - m_frame->y();
    int collapseBottom = qrect.bottom() - m_frame->frameGeometry().bottom();

    if (collapseTop > 0)
    {
        m_collapseTop = collapseTop;
        m_collapseBottom = collapseBottom;
        result = true;
    }
    return result;
}

bool OfficeFrameHelperWin::winEvent(MSG* message, long* result)
{
    if (m_hwndFrame && message->hwnd != m_hwndFrame && m_ribbonBar && message->hwnd == m_ribbonBar->winId())
    {
        if (message->message == WM_NCHITTEST)
        {
            POINT point;
            point.x = (short)qtn_GET_X_LPARAM((DWORD)message->lParam);
            point.y = (short)qtn_GET_Y_LPARAM((DWORD)message->lParam);

            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);
            rc.bottom = rc.top + titleBarSize();
            if (::PtInRect(&rc, point))
            {
                LRESULT lResult = 0;
                lResult = DefWindowProcW(m_hwndFrame, message->message, message->wParam, message->lParam);
                if (lResult == HTMINBUTTON || lResult == HTMAXBUTTON || lResult == HTCLOSE || lResult == HTHELP || lResult == HTCLIENT)
                {
                    *result = HTTRANSPARENT;
                    return true;
                }
            }
        }
        return false;
    }

    if (!m_officeFrameEnabled)
        return false;

    if (!m_dwmEnabled && m_lockNCPaint && (message->message == WM_STYLECHANGING || message->message == WM_STYLECHANGED
        || message->message == WM_WINDOWPOSCHANGED || message->message == WM_WINDOWPOSCHANGING || message->message == WM_NCPAINT))
    {
        if (message->message== WM_WINDOWPOSCHANGING)
            ((WINDOWPOS*)message->lParam)->flags &= ~SWP_FRAMECHANGED;

        return true;
    }
    else if (message->message == WM_WINDOWPOSCHANGING)
    {
        WINDOWPOS* lpwndpos = (WINDOWPOS*)message->lParam;

        QSize szFrameRegion(lpwndpos->cx, lpwndpos->cy);

        if (((lpwndpos->flags & SWP_NOSIZE) ==  0) && (m_szFrameRegion != szFrameRegion) &&
            ((getStyle() & WS_CHILD) == 0))
        {
            m_skipNCPaint = true;
        }

        if ((lpwndpos->flags == (SWP_NOACTIVATE | SWP_NOZORDER)) && collapseTopFrameStrut() && !m_wasFullScreen)
        {
            int offset = m_collapseTop + (m_dwmEnabled ? 0 : -m_frameBorder);
            lpwndpos->y += offset;
            lpwndpos->cy -= offset + m_collapseBottom*2 + 1;
        }

        return false;
    }
    else if (message->message == WM_WINDOWPOSCHANGED)
    {
        WINDOWPOS* lpwndpos = (WINDOWPOS*)message->lParam;
        if (lpwndpos->flags & SWP_FRAMECHANGED && !m_inUpdateFrame)
            updateFrameRegion();

        m_wasFullScreen = m_frame->windowState() & Qt::WindowFullScreen;

        return false;
    }
    else if (message->message == WM_SIZE || message->message == WM_STYLECHANGED)
    {
        RECT rc;
        ::GetWindowRect(m_hwndFrame, &rc);

        QSize szFrameRegion(rc.right - rc.left, rc.bottom - rc.top);

        if ((m_szFrameRegion != szFrameRegion) || (message->message == WM_STYLECHANGED))
        {
            updateFrameRegion(szFrameRegion, (message->message == WM_STYLECHANGED));
            redrawFrame();
        }

        if (message->message == WM_SIZE && message->wParam == SIZE_RESTORED && m_inLayoutRequest)
        {
            QResizeEvent e(m_ribbonBar->size(), m_ribbonBar->size());
            QApplication::sendEvent(m_ribbonBar, &e);
            m_inLayoutRequest = false;
        }

        if (message->message == WM_SIZE && message->wParam == SIZE_MINIMIZED)
        {
            if (getStyle(FALSE) & WS_MAXIMIZE)
                modifyStyle(m_hwndFrame, WS_MAXIMIZE, 0, 0);
        }
        return false;
    }
    else if (message->message == WM_ENTERSIZEMOVE)
    {
        m_inLayoutRequest = true;
    }
    else if (message->message == WM_NCRBUTTONUP)
    {
        // call the system menu
        ::SendMessageW(m_hwndFrame, 0x0313, (WPARAM)m_hwndFrame, message->lParam);
        return true;
    }
    else if (message->message == WM_SETTEXT)
    {
        *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);

        QEvent e(QEvent::WindowTitleChange);
        QApplication::sendEvent(m_ribbonBar, &e);
        return true;
    }
    else if (message->message == WM_NCHITTEST /*&& !isTitleVisible()*/ && !m_dwmEnabled)
    {
        if (!isTitleVisible())
        {
            *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);

            POINT point;
            point.x = (short)qtn_GET_X_LPARAM((DWORD)message->lParam);
            point.y = (short)qtn_GET_Y_LPARAM((DWORD)message->lParam);

            if (*result == HTCLIENT)
            {
                RECT rc;
                ::GetWindowRect(m_hwndFrame, &rc);

                rc.bottom = rc.top + m_frameBorder;
                if (::PtInRect(&rc, point))
                    *result = HTTOP;

                if (*result == HTCLIENT)
                {
                    QPoint pos(m_ribbonBar->mapFromGlobal(QPoint(point.x, point.y)));
                    if (m_ribbonBar && m_rcHeader.isValid())
                    {
                        if (!hitTestContextHeaders(pos) && m_rcHeader.contains(pos))
                            *result = HTCAPTION;
                    }
                    else
                    {
                        rc.bottom = rc.top + titleBarSize();
                        if (::PtInRect(&rc, point))
                            *result = HTCAPTION;
                    }

                    if (pos.y() < m_rcHeader.height() &&  m_ribbonBar->getSystemButton() && 
                        m_ribbonBar->getSystemButton()->toolButtonStyle() != Qt::ToolButtonFollowStyle)
                    {
                        DWORD dwStyle = getStyle();
                        if (pos.x() < 7 + GetSystemMetrics(SM_CXSMICON) && (dwStyle & WS_SYSMENU))
                            *result = HTSYSMENU;
                    }
                }
            }
        }
        else
        {
            *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);
            if (*result == HTCLOSE || *result == HTMAXBUTTON || *result == HTMINBUTTON || *result == HTHELP)
                *result = HTCAPTION;
        }

        return true;
    }
    else if (message->message == WM_NCHITTEST  && !isTitleVisible() && m_dwmEnabled)
    {
        LRESULT lResult;
        pDwmDefWindowProc(message->hwnd, message->message, message->wParam, message->lParam, &lResult);

        if (lResult == HTCLOSE || lResult == HTMAXBUTTON || lResult == HTMINBUTTON || lResult == HTHELP)
           *result = lResult;
        else
        {
            *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);

            bool buttonFollowStyle = m_ribbonBar->getSystemButton() && 
                m_ribbonBar->getSystemButton()->toolButtonStyle() == Qt::ToolButtonFollowStyle;

            if (HTSYSMENU == *result && buttonFollowStyle)
                *result = HTCLIENT;

            POINT point;
            point.x = (short)qtn_GET_X_LPARAM((DWORD)message->lParam);
            point.y = (short)qtn_GET_Y_LPARAM((DWORD)message->lParam);

            if (*result == HTCLIENT)
            {
                RECT rc;
                ::GetWindowRect(m_hwndFrame, &rc);
                rc.bottom = rc.top + m_frameBorder;

                if (::PtInRect(&rc, point))
                    *result = HTTOP;

                if (*result == HTCLIENT)
                {
                    if (m_ribbonBar && m_rcHeader.isValid())
                    {
                        QPoint pos(m_ribbonBar->mapFromGlobal(QPoint(point.x, point.y)));
                        if (!hitTestContextHeaders(pos) && m_rcHeader.adjusted(0,0,0,m_frameBorder).contains(pos))
                            *result = HTCAPTION;
                    }
                    else
                    {
                        rc.bottom = rc.top + titleBarSize();
                        if (::PtInRect(&rc, point))
                            *result = HTCAPTION;
                   }
                }
            }
            else
            {
                if (m_ribbonBar && m_rcHeader.isValid())
                {
                    QPoint pos(m_ribbonBar->mapFromGlobal(QPoint(point.x, point.y)));
                    QRect rectButtons = m_rcHeader.adjusted(0,0,0,m_frameBorder);
                    rectButtons.setLeft(rectButtons.right());
                    rectButtons.setRight(rectButtons.left()+ 100);
                    if (rectButtons.contains(pos))
                        *result = HTCAPTION;
                }
            }
        }
        return true;
    }
    else if (message->message == WM_NCACTIVATE && !m_dwmEnabled)
    {
        bool active = (bool)message->wParam;

        if (!::IsWindowEnabled(m_hwndFrame))
            active = true;

        DWORD dwStyle = getStyle();

        if (dwStyle & WS_VISIBLE)
        {
            refreshFrameStyle();

            m_lockNCPaint = true;

            if (dwStyle & WS_SIZEBOX)
                modifyStyle(m_hwndFrame, WS_SIZEBOX, 0, 0);

            if (::IsWindowEnabled(m_hwndFrame))
                *result = DefWindowProcW(message->hwnd, message->message, active, 0);
            else
                *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);

            if (dwStyle & WS_SIZEBOX)
                modifyStyle(m_hwndFrame, 0, WS_SIZEBOX, 0);

            m_lockNCPaint = false;

            if (m_active != active)
            {
                m_active = active;
                redrawFrame();
                m_ribbonBar->repaint();
                m_ribbonBar->update();
            }
        }
        else
        {
            m_active = active;
            return false;
        }
        return true;
    }
    else if (message->message == HLP_WM_DWMCOMPOSITIONCHANGED)
    {
        bool dwmEnabled = !m_allowDwm || getStyle() & WS_CHILD ? FALSE : isCompositionEnabled();

        if (dwmEnabled != m_dwmEnabled)
        {
            m_dwmEnabled = dwmEnabled;

            ::SetWindowRgn(m_hwndFrame, 0, true);

            if (!m_dwmEnabled)
                refreshFrameStyle();

            ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        return false;
    }
    else if (message->message == WM_NCCALCSIZE && !m_lockNCPaint)
    {
        NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS FAR*)message->lParam;

        RECT rc;
        rc.left    = lpncsp->rgrc[0].left;
        rc.top     = lpncsp->rgrc[0].top;
        rc.right   = lpncsp->rgrc[0].right;
        rc.bottom  = lpncsp->rgrc[0].bottom;

        if (m_dwmEnabled)
            *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);
        else
        {
            lpncsp->rgrc[0].left += m_frameBorder;
            lpncsp->rgrc[0].top += m_frameBorder;
            lpncsp->rgrc[0].right -= m_frameBorder;
            lpncsp->rgrc[0].bottom -= m_frameBorder-1;
        }

        DWORD dwStyle = getStyle();

        if (m_dwmEnabled)
        {
            if (!isTitleVisible())
                lpncsp->rgrc[0].top = rc.top;
        }
        else
        {
            lpncsp->rgrc[0].top += (isTitleVisible() ? titleBarSize() - m_frameBorder :  0);
            if (isFrameHasStatusBar() && ((dwStyle & WS_MAXIMIZE) == 0))
            {
                int borderSize = qMax(rc.bottom - lpncsp->rgrc[0].bottom - 3, 1L);
                m_borderSizeBotton = borderSize;
                lpncsp->rgrc[0].bottom = rc.bottom - borderSize;
            }
        }
//        if (((dwStyle & (WS_MAXIMIZE | WS_CHILD)) == WS_MAXIMIZE) && getAutoHideBar())
//            lpncsp->rgrc[0].bottom -= 1;

        return true;
    }
    else if (message->message == WM_SYSCOMMAND && !m_dwmEnabled && message->wParam == SC_MINIMIZE && getStyle() & WS_CHILD)
    {
         *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);

        redrawFrame();
        return true;
    }
    else if (message->message == WM_GETMINMAXINFO && !m_dwmEnabled)
    {
        *result = DefWindowProcW(message->hwnd, message->message, message->wParam, message->lParam);

        MINMAXINFO* lpMMI = (MINMAXINFO*)message->lParam;

        int yMin = m_frameBorder + titleBarSize();
        int xMin = (int)3 * yMin;

        xMin += GetSystemMetrics(SM_CYSIZE) + 2 * GetSystemMetrics(SM_CXEDGE);

        lpMMI->ptMinTrackSize.x = qMax(lpMMI->ptMinTrackSize.x, (LONG)xMin);
        lpMMI->ptMinTrackSize.y = qMax(lpMMI->ptMinTrackSize.y, (LONG)yMin);

        return true;
    }
    else if (message->message == WM_NCPAINT && !m_dwmEnabled)
    {
        if (m_skipNCPaint)
        {
            m_skipNCPaint = false;
            return true;
        }
        if (!isMDIMaximized())
        {
            redrawFrame();
            m_skipNCPaint = true;
        }
        return true;
    }
    return false;
}

bool OfficeFrameHelperWin::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_frame)
    {
        static bool lockUpdate = false;
        if (!lockUpdate && event->type() == QEvent::UpdateRequest)
        {
            lockUpdate = true;
            QRect rect = m_ribbonBar->rect();
            int cyTopHeight = 0;
            if (m_ribbonBar && m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_FlatFrame))
                cyTopHeight += m_ribbonBar->tabBarHeight() + 1;

            int yMin = m_frameBorder + titleBarSize() + cyTopHeight;
            rect.setBottom(rect.top() + yMin);
            m_ribbonBar->repaint(rect);

            lockUpdate = false;
        }
        else if (event->type() == QEvent::Resize /*&& isTitleVisible()*/)
        {
            if (m_collapseTop > 0)
            {
                QWidgetData* data = qt_qwidget_data(m_frame);
                data->crect.setTop(data->crect.top()-m_collapseTop);
                data->crect.setBottom(data->crect.bottom()-m_collapseTop);
                m_collapseBottom = -1;
                m_collapseTop = -1;
            }

            return true;
        }
        else if (event->type() == QEvent::StyleChange)
        {
            updateFrameRegion();
            redrawFrame();
        }
        else if (event->type() == QEvent::WindowStateChange)
        {
            if (m_frame->windowState() & Qt::WindowFullScreen)
            {
                m_ribbonBar->setFrameThemeEnabled(false);
                return QObject::eventFilter(obj, event);
            }

            bool updates = m_frame->updatesEnabled();
            if (updates)
                m_frame->setUpdatesEnabled(false);

            QApplication::postEvent(m_ribbonBar, new QEvent(QEvent::LayoutRequest));

            QSize s = m_frame->size();
            if (s.isValid())
                m_frame->resize(s);

            if (updates)
                m_frame->setUpdatesEnabled(updates);
        }
        else if (m_dwmEnabled && event->type() == QEvent::Show)
        {
            if (!(m_frame->windowState() & Qt::WindowMaximized) && !(m_frame->windowState() & Qt::WindowMinimized))
                ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }
    return QObject::eventFilter(obj, event);
}

bool OfficeFrameHelperWin::isCompositionEnabled()
{
    if (pDwmIsCompositionEnabled) 
    {
        BOOL enabled;
        HRESULT hr = pDwmIsCompositionEnabled(&enabled);
        return (SUCCEEDED(hr) && enabled);
    }
    return false;
}

bool OfficeFrameHelperWin::resolveSymbols()
{
    static bool tried = false;
    if (!tried) 
    {
        tried = true;
        QLibrary dwmLib(QString::fromAscii("dwmapi"));
        pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
        if (pDwmIsCompositionEnabled) 
        {
            pDwmDefWindowProc = (PtrDwmDefWindowProc)dwmLib.resolve("DwmDefWindowProc");
            pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
        }
        QLibrary themeLib(QString::fromAscii("uxtheme"));
        pIsAppThemed = (PtrIsAppThemed)themeLib.resolve("IsAppThemed");

        if (pIsAppThemed) 
        {
//            pDrawThemeBackground = (PtrDrawThemeBackground)themeLib.resolve("DrawThemeBackground");
//            pGetThemePartSize = (PtrGetThemePartSize)themeLib.resolve("GetThemePartSize");
//            pGetThemeColor = (PtrGetThemeColor)themeLib.resolve("GetThemeColor");
//            pIsThemeActive = (PtrIsThemeActive)themeLib.resolve("IsThemeActive");
            pOpenThemeData = (PtrOpenThemeData)themeLib.resolve("OpenThemeData");
//            pCloseThemeData = (PtrCloseThemeData)themeLib.resolve("CloseThemeData");
            pGetThemeSysFont = (PtrGetThemeSysFont)themeLib.resolve("GetThemeSysFont");
            pDrawThemeTextEx = (PtrDrawThemeTextEx)themeLib.resolve("DrawThemeTextEx");
//            pSetWindowThemeAttribute = (PtrSetWindowThemeAttribute)themeLib.resolve("SetWindowThemeAttribute");
        }
    }
/*
    return (pDwmIsCompositionEnabled != 0 && pDwmDefWindowProc != 0 && pDwmExtendFrameIntoClientArea != 0 && 
            pIsAppThemed != 0 && pDrawThemeBackground != 0 && pGetThemePartSize != 0 && pGetThemeColor != 0 && 
            pIsThemeActive != 0 && pOpenThemeData != 0 && pCloseThemeData != 0 && pGetThemeSysFont != 0 && 
            pDrawThemeTextEx != 0 && pSetWindowThemeAttribute != 0);
*/
    return (pIsAppThemed != 0 && /*pDrawThemeBackground != 0 && pGetThemePartSize != 0 && pGetThemeColor != 0 &&
            pIsThemeActive != 0 &&*/ pOpenThemeData != 0 && /*pCloseThemeData != 0 &&*/ pGetThemeSysFont != 0);
}

