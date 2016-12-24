/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced MainWindow for Qt)
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
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qhash.h>
#include <qstyle.h>
#include <QIcon>
#include <qstyleoption.h>
#include <qstylepainter.h>
#include <qtextdocument.h>
#include <QToolTip>
#include <QBitmap>
#include <qdebug.h>

#include "QtnRibbonToolTip.h"
#include "QtnRibbonStyle.h"


static int keyTipEventNumber = -1;
static int showKeyTipEventNumber = -1;
static int hideKeyTipEventNumber = -1;

using namespace Qtitan;

/* KeyTipEvent */
KeyTipEvent::KeyTipEvent(RibbonKeyTip* kTip) 
    : QEvent(KeyTipEvent::eventNumber())
{
    keyTip = kTip;
}

RibbonKeyTip* KeyTipEvent::getKeyTip() const
{
    return keyTip;
}

QEvent::Type KeyTipEvent::eventNumber()
{
    if (keyTipEventNumber < 0)
        keyTipEventNumber = QEvent::registerEventType();
    return (QEvent::Type) keyTipEventNumber;
}

/* ShowKeyTipEvent */
ShowKeyTipEvent::ShowKeyTipEvent(QWidget* w) 
    : QEvent(ShowKeyTipEvent::eventNumber())
{
    widget = w;
}

QWidget* ShowKeyTipEvent::getWidget() const
{
    return widget;
}

QEvent::Type ShowKeyTipEvent::eventNumber()
{
    if (showKeyTipEventNumber < 0)
        showKeyTipEventNumber = QEvent::registerEventType();
    return (QEvent::Type) showKeyTipEventNumber;
}

/* ShowKeyTipEvent */
HideKeyTipEvent::HideKeyTipEvent() 
    : QEvent(HideKeyTipEvent::eventNumber())
{
}

QEvent::Type HideKeyTipEvent::eventNumber()
{
    if (hideKeyTipEventNumber < 0)
        hideKeyTipEventNumber = QEvent::registerEventType();
    return (QEvent::Type) hideKeyTipEventNumber;
}

struct QEffects
{
    enum Direction {
        LeftScroll  = 0x0001,
        RightScroll = 0x0002,
        UpScroll    = 0x0004,
        DownScroll  = 0x0008
    };

    typedef uint DirFlags;
};

extern void Q_GUI_EXPORT qScrollEffect(QWidget*, QEffects::DirFlags dir = QEffects::DownScroll, int time = -1);
extern void Q_GUI_EXPORT qFadeEffect(QWidget*, int time = -1);


namespace Qtitan
{

    /* RibbonToolTipPrivate */
    class RibbonToolTipPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonToolTip)
    public:
        explicit RibbonToolTipPrivate();

    public:
        void updateTool();

    public:
        int margin_;
        int indent_;
        int nMaxTipWidth_;
        QBasicTimer hideTimer_, expireTimer_;
        bool fadingOut_;
        QWidget* widget_;
        QRect rect_;
        QString text_;
        QString textTitle_;
        QIcon icon_;
        static RibbonToolTip* instance_;
        QLabel* label_;

    };
};

RibbonToolTip* RibbonToolTipPrivate::instance_ = Q_NULL;
RibbonToolTipPrivate::RibbonToolTipPrivate()
{
    margin_ = 0;
    fadingOut_ = false;
    widget_ = Q_NULL;
    indent_ = -1;
    nMaxTipWidth_ = 160;
}

void RibbonToolTipPrivate::updateTool()
{
    QTN_P(RibbonToolTip);
    label_->setText(text_);
    label_->setWordWrap(Qt::mightBeRichText(text_));
    label_->adjustSize();

    p.updateGeometry();
    p.update(p.contentsRect());
}


/* RibbonToolTip */
RibbonToolTip::RibbonToolTip(const QString& textTitle, const QString& text, const QIcon& icon, QWidget* w)
    : QFrame(w, Qt::ToolTip | Qt::BypassGraphicsProxyWidget)
{
    delete RibbonToolTipPrivate::instance_;
    RibbonToolTipPrivate::instance_ = this;

    QTN_INIT_PRIVATE(RibbonToolTip);
    QTN_D(RibbonToolTip);

    d.label_ = new QLabel(this);
    d.label_->setForegroundRole(QPalette::ToolTipText);
    d.label_->setBackgroundRole(QPalette::ToolTipBase);
    d.label_->setPalette(QToolTip::palette());
    d.label_->setFont(QToolTip::font());
    d.label_->ensurePolished();
    d.label_->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    d.label_->setFrameStyle(QFrame::NoFrame);

    QWidgetData* pDate = qt_qwidget_data(this);
    d.nMaxTipWidth_ = pDate->crect.width()/4;

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(QToolTip::palette());
    setFont(QToolTip::font());
    ensurePolished();
    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    setFrameStyle(QFrame::NoFrame);
    qApp->installEventFilter(this);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
    setMouseTracking(true);
    d.fadingOut_ = false;
    setIcon(icon);
    reuseTip(textTitle, text);
}

RibbonToolTip::~RibbonToolTip()
{
    RibbonToolTipPrivate::instance_ = Q_NULL;
    QTN_FINI_PRIVATE();
}

RibbonToolTip* RibbonToolTip::instance()
{
    return RibbonToolTipPrivate::instance_;
}

void RibbonToolTip::showToolTip(const QPoint &pos, const QString& textTitle, const QString& text, const QIcon& icon, QWidget* w)
{
    QRect rect;
    // a tip does already exist
    if (RibbonToolTipPrivate::instance_ && RibbonToolTipPrivate::instance_->isVisible())
    {
        // empty text means hide current tip
        if (text.isEmpty())
        {
            RibbonToolTipPrivate::instance_->hideTip();
            return;
        }
        else if (!RibbonToolTipPrivate::instance_->fadingOut())
        {
            // If the tip has changed, reuse the one
            // that is showing (removes flickering)
            QPoint localPos = pos;
            if (w)
                localPos = w->mapFromGlobal(pos);
            if (RibbonToolTipPrivate::instance_->tipChanged(localPos, text, textTitle, w))
            {
                RibbonToolTipPrivate::instance_->setIcon(icon);
                RibbonToolTipPrivate::instance_->reuseTip(textTitle, text);
                RibbonToolTipPrivate::instance_->setTipRect(w, rect);
                RibbonToolTipPrivate::instance_->placeTip(pos, w);
            }
            return;
        }
    }
    // no tip can be reused, create new tip:
    if (!text.isEmpty())
    {
#ifndef Q_WS_WIN
        new RibbonToolTip(textTitle, text, icon, w); // sets TipLabel::m_pInstanceto itself
#else
        // On windows, we can't use the widget as parent otherwise the window will be
        // raised when the tooltip will be shown
        new RibbonToolTip(textTitle, text, icon, QApplication::desktop()->screen(RibbonToolTip::getTipScreen(pos, w)));
#endif
        RibbonToolTipPrivate::instance_->setTipRect(w, rect);
        RibbonToolTipPrivate::instance_->placeTip(pos, w);
        RibbonToolTipPrivate::instance_->setObjectName(QLatin1String("qtntooltip_label"));


#if !defined(QT_NO_EFFECTS) && !defined(Q_WS_MAC)
        if (QApplication::isEffectEnabled(Qt::UI_FadeTooltip))
            qFadeEffect(RibbonToolTipPrivate::instance_);
        else if (QApplication::isEffectEnabled(Qt::UI_AnimateTooltip))
            qScrollEffect(RibbonToolTipPrivate::instance_);
        else
            RibbonToolTipPrivate::instance_->show();
#else
        RibbonToolTipPrivate::instance_->show();
#endif
    }
}

void RibbonToolTip::hideToolTip() 
{ 
    showToolTip(QPoint(), QString(), QString(), QIcon()); 
}

bool RibbonToolTip::isVisibleToolTip()
{
    return (RibbonToolTipPrivate::instance_ != 0 && RibbonToolTipPrivate::instance_->isVisible());
}

QString RibbonToolTip::textToolTip()
{
    if (RibbonToolTipPrivate::instance_)
        return RibbonToolTipPrivate::instance_->text();
    return QString();
}

int RibbonToolTip::margin() const
{
    QTN_D(const RibbonToolTip);
    return d.margin_;
}

void RibbonToolTip::setMargin(int margin)
{
    QTN_D(RibbonToolTip);
    if (d.margin_ == margin)
        return;
    d.margin_ = margin;
    d.updateTool();
}

void RibbonToolTip::setTitle(const QString& text)
{
    QTN_D(RibbonToolTip);
    if (d.textTitle_ == text)
        return;
    d.textTitle_ = text;
    d.updateTool();
}

const QString& RibbonToolTip::title() const
{
    QTN_D(const RibbonToolTip);
    return d.textTitle_;
}

void RibbonToolTip::setText(const QString& text)
{
    QTN_D(RibbonToolTip);
    if (d.text_ == text)
        return;
    d.text_ = text;
    d.updateTool();
}

const QString& RibbonToolTip::text() const
{
    QTN_D(const RibbonToolTip);
    return d.text_;
}

void RibbonToolTip::setIcon(const QIcon& icon)
{
    QTN_D(RibbonToolTip);
    d.icon_ = icon;
    d.updateTool();
}

const QIcon& RibbonToolTip::icon() const
{
    QTN_D(const RibbonToolTip);
    return d.icon_;
}

void RibbonToolTip::restartExpireTimer()
{
    QTN_D(RibbonToolTip);
    int time = 10000 + 40 * qMax(0, text().length()-100);
    d.expireTimer_.start(time, this);
    d.hideTimer_.stop();
}

void RibbonToolTip::reuseTip(const QString& textTitle, const QString& text)
{
    setText(text);
    setTitle(textTitle);
    QFontMetrics fm(font());
    QSize extra(1, 0);
    // Make it look good with the default RibbonToolTip font on Mac, which has a small descent.
    if (fm.descent() == 2 && fm.ascent() >= 11)
        ++extra.rheight();

    resize(sizeHint() + extra);
    restartExpireTimer();
}

QSize RibbonToolTip::sizeHint() const
{
    QTN_D(const RibbonToolTip);

    QFontMetrics fm = fontMetrics();

//    int nMaxTipWidth = d.nMaxTipWidth_;

    QRect rcMargin(margin(), margin(), margin(), margin());
    QSize szMargin(3 + rcMargin.left() + rcMargin.right() + 3, 3 + rcMargin.top() + rcMargin.bottom() + 3);

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d.textTitle_.isEmpty();
    bool drawImage = !d.icon_.isNull();
    bool drawImageTop = true;
    QSize szImage(0, 0);
    QSize szTitle(0, 0);

    if (drawTitle)
    {
        int w = fm.averageCharWidth() * 80;
        QRect rcTitle = fm.boundingRect(0, 0, w, 2000, flags | Qt::TextSingleLine, d.textTitle_);
        szTitle = QSize(rcTitle.width() + 4, rcTitle.height() + 20);
    }

    if (drawImage)
    {
        QSize szIcon(0, 0);
        if (!d.icon_.availableSizes().isEmpty())
            szIcon = d.icon_.availableSizes().first();

        drawImageTop = (szIcon.height() <= 16);

        if (drawImageTop)
        {
            if (!drawTitle)
                szImage.setWidth(szIcon.height() + 3);
            else
                szTitle.setWidth(szTitle.width() + szIcon.width() + 1);
        }
        else
            szImage.setWidth(szIcon.width() + 5);
        szImage.setHeight(szIcon.height());
    }

//    QRect rcText(QPoint(0, 0), d.label_->sizeHint());
//    QRect rcText(QPoint(0, 0), QSize(nMaxTipWidth - szMargin.width(), 0));
//    rcText = fm.boundingRect(rcText.left(), rcText.top(), rcText.width(), rcText.height(), flags | Qt::TextWordWrap, d.text_);
//    QSize szText = rcText.size();
    QSize szText = d.label_->sizeHint();

    QSize sz(0, 0);
    sz.setHeight(qMax(szImage.height(), szText.height()));
    sz.setWidth(szImage.width() + szText.width());

    if (drawTitle)
    {
        sz.setWidth(qMax(sz.width() + 25, szTitle.width()));
        sz.setHeight(sz.height() + szTitle.height());
    }

    sz += szMargin;
    return sz;
}

void RibbonToolTip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QTN_D(RibbonToolTip);

    QStylePainter p(this);
    drawFrame(&p);

    QStyleOptionFrame opt;
    opt.init(this);

    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    
    QRect rc = contentsRect();
    rc.adjust(margin(), margin(), -margin(), -margin());
    rc.adjust(3, 3, -3, -3);

    QRect rcTitle(rc.left() + 2, rc.top() + 2, rc.right() - 2, rc.bottom());

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d.textTitle_.isEmpty();
    bool drawImage = !d.icon_.isNull();
    bool drawImageTop = true;
    QSize szImage(0, 0);

    if (drawImage)
    {
        if (!d.icon_.availableSizes().isEmpty())
            szImage = d.icon_.availableSizes().first();

        drawImageTop = (szImage.height() <= 16);

        if (drawImageTop)
        {
            QPoint ptIcon = rc.topLeft();
            QPixmap pm = d.icon_.pixmap(szImage, QIcon::Normal, QIcon::On);
            p.drawPixmap(ptIcon, pm);

            if (drawTitle) 
                rcTitle.setLeft(rcTitle.left()+szImage.width() + 1); 
            else 
                rc.setLeft(rc.left()+szImage.width() + 3);
        }
    }
    if (drawTitle)
    {
        QFont oldFont = p.font();
        QFont fnt = oldFont;
        fnt.setBold(true);
        p.setFont(fnt);
        style()->drawItemText(&p, rcTitle, flags | Qt::TextSingleLine, opt.palette, isEnabled(), d.textTitle_, QPalette::ToolTipText);
        p.setFont(oldFont);

        rc.setTop(rc.top() + p.boundingRect(rcTitle, flags | Qt::TextSingleLine, d.textTitle_).height());
        rc.adjust(10, 12, -15, 0);
    }

    if (drawImage && !drawImageTop)
    {
        QPoint ptIcon = rc.topLeft();
        QPixmap pm = d.icon_.pixmap(szImage, QIcon::Normal, QIcon::On);
        p.drawPixmap(ptIcon, pm);
        rc.setLeft(rc.left() + szImage.width() + 5);
    }

    d.label_->move(rc.topLeft());
//    style()->drawItemText(&p, rc, flags | Qt::TextWordWrap, opt.palette, isEnabled(), d.text_, QPalette::ToolTipText);
    p.end();
}

void RibbonToolTip::resizeEvent(QResizeEvent* event)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);

    QFrame::resizeEvent(event);
}

void RibbonToolTip::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(RibbonToolTip);
    if (d.rect_.isNull())
        return;
    QPoint pos = event->globalPos();
    if (d.widget_)
        pos = d.widget_->mapFromGlobal(pos);
    if (!d.rect_.contains(pos))
        hideTip();
    QFrame::mouseMoveEvent(event);
}

void RibbonToolTip::hideTip()
{
    QTN_D(RibbonToolTip);
    if (!d.hideTimer_.isActive())
        d.hideTimer_.start(300, this);
}

void RibbonToolTip::closeToolTip()
{
    close(); // to trigger QEvent::Close which stops the animation
    deleteLater();
}

void RibbonToolTip::setTipRect(QWidget* w, const QRect& r)
{
    QTN_D(RibbonToolTip);
    if (!d.rect_.isNull() && !w)
        qWarning("RibbonToolTip::setTipRect: Cannot pass null widget if rect is set");
    else
    {
        d.widget_ = w;
        d.rect_ = r;
    }
}

void RibbonToolTip::timerEvent(QTimerEvent* event)
{
    QTN_D(RibbonToolTip);
    if (event->timerId() == d.hideTimer_.timerId() || event->timerId() == d.expireTimer_.timerId())
    {
        d.hideTimer_.stop();
        d.expireTimer_.stop();
        closeToolTip();
    }
}

bool RibbonToolTip::eventFilter(QObject *o, QEvent* event)
{
    QTN_D(RibbonToolTip);
    switch (event->type()) 
    {
        case QEvent::Leave:
                hideTip();
            break;
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Wheel:
                closeToolTip();
            break;
        case QEvent::MouseMove:
            if (o == d.widget_ && !d.rect_.isNull() && !d.rect_.contains(static_cast<QMouseEvent*>(event)->pos()))
                hideTip();
        default:
            break;
    }
    return false;
}

int RibbonToolTip::getTipScreen(const QPoint& pos, QWidget* w)
{
    if (QApplication::desktop()->isVirtualDesktop())
        return QApplication::desktop()->screenNumber(pos);
    else
        return QApplication::desktop()->screenNumber(w);
}

void RibbonToolTip::placeTip(const QPoint& pos, QWidget* w)
{
    QRect screen = QApplication::desktop()->screenGeometry(getTipScreen(pos, w));

    QPoint p = pos;
    if (p.x() + this->width() > screen.x() + screen.width())
        p.rx() -= 4 + this->width();
    if (p.y() + this->height() > screen.y() + screen.height())
        p.ry() -= 24 + this->height();
    if (p.y() < screen.y())
        p.setY(screen.y());
    if (p.x() + this->width() > screen.x() + screen.width())
        p.setX(screen.x() + screen.width() - this->width());
    if (p.x() < screen.x())
        p.setX(screen.x());
    if (p.y() + this->height() > screen.y() + screen.height())
        p.setY(screen.y() + screen.height() - this->height());

    this->move(p);
}

bool RibbonToolTip::tipChanged(const QPoint& pos, const QString& text, const QString& textTitle, QObject* o)
{
    QTN_D(RibbonToolTip);
    if (RibbonToolTipPrivate::instance_->text() != text)
        return true;

    if (RibbonToolTipPrivate::instance_->title() != textTitle)
        return true;

    if (o != d.widget_)
        return true;

    if (!d.rect_.isNull())
        return !d.rect_.contains(pos);
    else
        return false;
}

bool RibbonToolTip::fadingOut() const
{
    QTN_D(const RibbonToolTip);
    return d.fadingOut_;
}


namespace Qtitan
{
    /* RibbonKeyTipPrivate*/
    class RibbonKeyTipPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonKeyTip)
    public:
        explicit RibbonKeyTipPrivate();
    public:
        QString strTip_;
        QString strCaption_;
        QString strPrefix_;
        QWidget* owner_;
        QAction* action_;
        QPoint pt_;
        bool enabled_;
        bool visible_;
        QWidget* bars_;
        uint align_;
        bool explicit_;
    };
};

RibbonKeyTipPrivate::RibbonKeyTipPrivate()
{
    owner_ = Q_NULL;
    action_ = Q_NULL;
    bars_ = Q_NULL;
    enabled_ = false;
    align_ = 0;
    visible_ = true;
    explicit_ = false;
}

/* KeyTip */
RibbonKeyTip::RibbonKeyTip(QWidget* bars, QWidget* owner, const QString& caption, 
    const QPoint& pt, uint align, bool enabled, QAction* action)
    : QFrame(bars, Qt::ToolTip | Qt::BypassGraphicsProxyWidget)
{
    QTN_INIT_PRIVATE(RibbonKeyTip);
    QTN_D(RibbonKeyTip);
    d.action_ = action;
    d.owner_ = owner;
    d.strCaption_ = caption;
    d.bars_ = bars;
    d.pt_ = pt;
    d.enabled_ = enabled;
    d.align_ = align;

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    QPalette pal = QToolTip::palette();
    pal.setColor(QPalette::Light, pal.color(QPalette::ToolTipText));
    setPalette(pal);
    setFont(QToolTip::font());
    ensurePolished();

    if (!enabled)
        setWindowOpacity(0.5);
}

RibbonKeyTip::~RibbonKeyTip()
{
    QTN_FINI_PRIVATE();
}

QPoint RibbonKeyTip::posTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.pt_;
}

void RibbonKeyTip::setVisibleTip(bool visible)
{
    QTN_D(RibbonKeyTip);
    d.visible_ = visible;
}

bool RibbonKeyTip::isVisibleTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.visible_;
}

bool RibbonKeyTip::isEnabledTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.enabled_;
}

QWidget* RibbonKeyTip::getBars() const
{
    QTN_D(const RibbonKeyTip);
    return d.bars_;
}

uint RibbonKeyTip::getAlign() const
{
    QTN_D(const RibbonKeyTip);
    return d.align_;
}

void RibbonKeyTip::setExplicit(bool exp)
{
    QTN_D(RibbonKeyTip);
    d.explicit_ = exp;
}

bool RibbonKeyTip::isExplicit() const
{
    QTN_D(const RibbonKeyTip);
    return d.explicit_;
}

QString RibbonKeyTip::getStringTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.strTip_;
}

void RibbonKeyTip::setStringTip(const QString& str)
{
    QTN_D(RibbonKeyTip);
    d.strTip_ = str;
}

QString RibbonKeyTip::getCaption() const
{
    QTN_D(const RibbonKeyTip);
    return d.strCaption_;
}

void RibbonKeyTip::setCaption(const QString& str)
{
    QTN_D(RibbonKeyTip);
    d.strCaption_ = str;
}

QString RibbonKeyTip::getPrefix() const
{
    QTN_D(const RibbonKeyTip);
    return d.strPrefix_;
}

void RibbonKeyTip::setPrefix(const QString& pref)
{
    QTN_D(RibbonKeyTip);
    d.strPrefix_ = pref;
}

QWidget* RibbonKeyTip::getOwner() const
{
    QTN_D(const RibbonKeyTip);
    return d.owner_;
}

QAction* RibbonKeyTip::getAction() const
{
    QTN_D(const RibbonKeyTip);
    return d.action_;
}

void RibbonKeyTip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonKeyTip);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);

    if (d.enabled_)
        opt.state |= QStyle::State_Enabled;
    else
        opt.state &= ~QStyle::State_Enabled;

    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonKeyTip, &opt, &p, this);
    style()->drawItemText(&p, opt.rect, Qt::AlignVCenter | Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap, 
        opt.palette, d.enabled_, d.strTip_, QPalette::ToolTipText);
}

void RibbonKeyTip::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    QBitmap maskBitmap(width(), height());
    maskBitmap.clear();

    QPainter p(&maskBitmap);
    p.setBrush(Qt::black);
    p.drawRoundedRect(0, 0, width()-1, height()-1, 2, 2, Qt::AbsoluteSize);
    p.end();
    setMask(maskBitmap);
}

