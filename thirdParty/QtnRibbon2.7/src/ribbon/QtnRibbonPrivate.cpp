/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonPrivate for Qt)
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
#include <QStyleOption>
#include <QResizeEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>
#include <QLayout>
#include <QEvent>

#include "QtnRibbonPrivate.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonPage.h"

using namespace Qtitan;

namespace Qtitan
{
    /* ExWidgetWrapperPrivate */
    class ExWidgetWrapperPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(ExWidgetWrapper)
    public:
        explicit ExWidgetWrapperPrivate();

    public:
        void init(QWidget* widget);

    public:
        QIcon icon_;
        QWidget* widget_;
        QString labelText_;
        int length_;
        bool align_;
    };
}; //namespace Qtitan

/* RibbonGroupPrivate */
ExWidgetWrapperPrivate::ExWidgetWrapperPrivate()
    : widget_(Q_NULL)
{
}

void ExWidgetWrapperPrivate::init(QWidget* widget)
{
    Q_ASSERT(widget != Q_NULL);
    QTN_P(ExWidgetWrapper);

    widget_ = widget;
    align_ = false;
    length_ = -1;
    widget_->setParent(&p);

    p.setMinimumWidth(widget_->minimumWidth());
    widget->setMinimumWidth(0);
    p.setMaximumWidth(widget_->maximumWidth());

    p.setFocusPolicy(widget_->focusPolicy());
    p.setAttribute(Qt::WA_InputMethodEnabled);
    p.setSizePolicy(widget_->sizePolicy());
    p.setMouseTracking(true);
    p.setAcceptDrops(true);
    p.setAttribute(Qt::WA_MacShowFocusRect, true);

    widget_->setFocusProxy(&p);
    widget_->setAttribute(Qt::WA_MacShowFocusRect, false);

    p.setToolTip(widget_->toolTip());
    widget_->setToolTip("");
}

/* ExWidgetWrapper */
ExWidgetWrapper::ExWidgetWrapper(QWidget* parent, QWidget* widget)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(ExWidgetWrapper);
    QTN_D(ExWidgetWrapper);
    d.init(widget);
}

ExWidgetWrapper::~ExWidgetWrapper()
{
    QTN_FINI_PRIVATE();
}

QString ExWidgetWrapper::labelText() const
{
    QTN_D(const ExWidgetWrapper);
    return d.labelText_;
}

void ExWidgetWrapper::setLabelText(const QString& text)
{
    QTN_D(ExWidgetWrapper);
    d.labelText_ = text;
}

void ExWidgetWrapper::setIcon(const QIcon& icon)
{
    QTN_D(ExWidgetWrapper);
    d.icon_ = icon;
}

QIcon ExWidgetWrapper::icon() const
{
    QTN_D(const ExWidgetWrapper);
    return d.icon_;
}

void ExWidgetWrapper::setAlignWidget(bool align)
{
    QTN_D(ExWidgetWrapper);
    d.align_ = align;
}

bool ExWidgetWrapper::alignWidget() const
{
    QTN_D(const ExWidgetWrapper);
    return d.align_;
}

void ExWidgetWrapper::setLengthLabel(int length)
{
    QTN_D(ExWidgetWrapper);
    d.length_ = length;
}

void ExWidgetWrapper::resizeEvent(QResizeEvent* event)
{
    updateGeometries();
    QWidget::resizeEvent(event);
}

void ExWidgetWrapper::updateGeometries()
{
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);

    QRect rect = panel.rect;

    QTN_D(ExWidgetWrapper);
    if (!d.icon_.isNull())
    {
        QSize sz = d.icon_.actualSize(QSize(16, 16));
        rect.adjust(sz.width(), 0, 0, 0);
    }

    if (!d.labelText_.isEmpty())
    {
        if (d.length_ == -1)
        {
            QFontMetrics fm = fontMetrics();
            QSize sz = fm.size(Qt::TextHideMnemonic, d.labelText_);
            int width =  sz.width() + fm.width(QLatin1Char('x'));
            rect.adjust(width, 0, 0, 0);
            d.widget_->setGeometry(rect);
        }
        else
        {
            rect.adjust(d.length_ + 1, 0, 0, 0);
            d.widget_->setGeometry(rect);
        }
    }
}

void ExWidgetWrapper::initStyleOption(QStyleOptionFrameV2* option) const
{
    QTN_D(const ExWidgetWrapper);

    option->initFrom(this);
    option->rect = contentsRect();
    option->lineWidth = qobject_cast<QLineEdit*>(d.widget_) ? style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this) : 0;
    option->midLineWidth = 0;
    option->state |= QStyle::State_Sunken;

    if (isReadOnly())
        option->state |= QStyle::State_ReadOnly;

#ifdef QT_KEYPAD_NAVIGATION
    if (hasEditFocus())
        option->state |= QStyle::State_HasEditFocus;
#endif
    option->features = QStyleOptionFrameV2::None;
}

bool ExWidgetWrapper::isReadOnly() const
{
    QTN_D(const ExWidgetWrapper);

    bool readOnly = false;
    if (QLineEdit* pEdit = qobject_cast<QLineEdit*>(d.widget_))
        readOnly = pEdit->isReadOnly();
    else if (QComboBox* pComboBox = qobject_cast<QComboBox*>(d.widget_))
        readOnly = pComboBox->isEditable();
    else if (QAbstractSpinBox* pSpinBox = qobject_cast<QAbstractSpinBox*>(d.widget_))
        readOnly = pSpinBox->isReadOnly();

    return readOnly;
}

void ExWidgetWrapper::focusInEvent(QFocusEvent* event)
{
    QTN_D(ExWidgetWrapper);
    QCoreApplication::sendEvent(d.widget_, event);
    QWidget::focusInEvent(event);
}

void ExWidgetWrapper::focusOutEvent(QFocusEvent* event)
{
    QTN_D(ExWidgetWrapper);
    QCoreApplication::sendEvent(d.widget_, event);
    QWidget::focusOutEvent(event);
}

bool ExWidgetWrapper::event(QEvent* event)
{
    QTN_D(ExWidgetWrapper);

    if (!d.widget_)
        return false;

    switch(event->type()) 
    {
        case QEvent::ShortcutOverride:
        case QEvent::KeyPress :
            {
                class QtnWidget : public QWidget { friend class Qtitan::ExWidgetWrapper; };
                return ((QtnWidget*)d.widget_)->event(event);
            }
        default:
            break;
    }
    return QWidget::event(event);
}

void ExWidgetWrapper::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(ExWidgetWrapper);

    QPainter p(this);
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);

    if (!d.icon_.isNull())
    {
        QRect rc = panel.rect;
        QPixmap pm;
        QSize pmSize = d.icon_.actualSize(QSize(16, 16));
        QIcon::State state = panel.state & QStyle::State_On ? QIcon::On : QIcon::Off;
        QIcon::Mode mode;
        if (!(panel.state & QStyle::State_Enabled))
            mode = QIcon::Disabled;
        else if ((panel.state & QStyle::State_MouseOver) && (panel.state & QStyle::State_AutoRaise))
            mode = QIcon::Active;
        else
            mode = QIcon::Normal;
        pm = d.icon_.pixmap(panel.rect.size().boundedTo(pmSize), mode, state);
        pmSize = pm.size();

        rc.setWidth(pmSize.width());

        style()->drawItemPixmap(&p, rc, Qt::AlignCenter, pm);
        panel.rect.setLeft(panel.rect.left() + pmSize.width() + 3);
    }
    if (!d.labelText_.isEmpty())
    {
        style()->drawItemText(&p, panel.rect, Qt::AlignLeft | Qt::AlignVCenter, panel.palette, 
            panel.state & QStyle::State_Enabled, d.labelText_, QPalette::ButtonText);
    }
}

QVariant ExWidgetWrapper::inputMethodQuery(Qt::InputMethodQuery property) const
{
    QTN_D(const ExWidgetWrapper);
    return d.widget_->inputMethodQuery(property);
}

QSize ExWidgetWrapper::sizeHint() const
{
    QTN_D(const ExWidgetWrapper);
    QSize sz = d.widget_->sizeHint();
    return sz;
}

void ExWidgetWrapper::inputMethodEvent(QInputMethodEvent* event)
{
    QTN_D(const ExWidgetWrapper);
    QCoreApplication::sendEvent(d.widget_, (QEvent*)event);
}




/* RibbonGroupPopupMenu */
ReducedGroupPopupMenu::ReducedGroupPopupMenu(QWidget* p)
    : QMenu(p)
    , m_group(Q_NULL)
    , m_parent(Q_NULL)
{
}

ReducedGroupPopupMenu::~ReducedGroupPopupMenu()
{
}

void ReducedGroupPopupMenu::setGroup(RibbonGroup* gr)
{
    m_group = gr;
    m_parent = m_group->parentWidget();
    m_group->setParent(this);
    connect(m_group, SIGNAL(actionTriggered(QAction*)), this, SLOT(actionTriggered(QAction*)));

    m_group->setFocusProxy(this);
    m_group->show();
}

RibbonGroup* ReducedGroupPopupMenu::getGroup() const
{
    return m_group;
}

void ReducedGroupPopupMenu::actionTriggered(QAction *action)
{
    Q_UNUSED(action);
    hide();
}

QSize ReducedGroupPopupMenu::sizeHint() const
{
    if(m_group) 
        return m_group->sizeHint();
    return QMenu::sizeHint();
}

void ReducedGroupPopupMenu::showEvent(QShowEvent* event)
{
    m_group->show();
    QMenu::showEvent(event);
}

void ReducedGroupPopupMenu::hideEvent(QHideEvent* event)
{
    m_group->disconnect(this);
    m_group->setParent(m_parent);
    m_parent = Q_NULL;
    m_group = Q_NULL;
    parentWidget()->update();
    QMenu::hideEvent(event);
}

void ReducedGroupPopupMenu::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
//    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonBar, &opt, &p, this);
    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this);
}

void ReducedGroupPopupMenu::resizeEvent(QResizeEvent* event)
{
    QMenu::resizeEvent(event);
}



/* RibbonGroupWrapper */
RibbonGroupWrapper::RibbonGroupWrapper(QWidget* parent)
    : QToolButton(parent)
{
    m_ribbonGroup = Q_NULL;
    m_reduced = false;

    setMenu(new ReducedGroupPopupMenu(this));
    setAttribute(Qt::WA_LayoutUsesWidgetRect);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setPopupMode(QToolButton::MenuButtonPopup);
}

RibbonGroupWrapper::~RibbonGroupWrapper()
{
    if (m_ribbonGroup)
        m_ribbonGroup->removeEventFilter(this);
}

void RibbonGroupWrapper::setGroup(RibbonGroup* rg)
{
    m_ribbonGroup = rg;
    m_ribbonGroup->installEventFilter(this);
    setText(m_ribbonGroup->title());
}

RibbonGroup* RibbonGroupWrapper::getGroup() const
{
    return m_ribbonGroup;
}

void RibbonGroupWrapper::setReducedGroup(bool reduced)
{
    if (m_ribbonGroup->isVisible())
    {
        m_reduced = reduced;
        m_ribbonGroup->hide();
        resize(sizeHint());
    }
}

bool RibbonGroupWrapper::isReducedGroup() const 
{ 
    return m_reduced; 
}

void RibbonGroupWrapper::resetReducedGroup()
{
    if (m_reduced)
    {
        m_reduced = false;
        m_ribbonGroup->show();
        m_ribbonGroup->resize(m_realSize);
    }
}

QSize RibbonGroupWrapper::realSize() const
{
    return m_realSize;
}

QSize RibbonGroupWrapper::reducedSize() const
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    return QSize(style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonReducedGroupWidth, &opt, this), m_realSize.height());
}

void RibbonGroupWrapper::updateLayout(int* width, bool reset)
{
    if (!m_ribbonGroup)
        return;

    if (reset)
        m_ribbonGroup->resetLayout();

    if (width)
    {
        if (m_ribbonGroup->extendSize(*width))
        {
            m_ribbonGroup->layout()->update();
            m_ribbonGroup->layout()->activate();
        }
    }
    else
    {
        m_ribbonGroup->layout()->update();
        m_ribbonGroup->layout()->activate();
    }

    if (reset)
        m_realSize = m_ribbonGroup->sizeHint();
}

QSize RibbonGroupWrapper::sizeHint() const
{
    RibbonGroupWrapper* that = const_cast<RibbonGroupWrapper*>(this);

    that->m_realSize = m_ribbonGroup ? m_ribbonGroup->sizeHint() : QSize();
    if (!m_reduced)
        return m_realSize;
    return reducedSize();
}

bool RibbonGroupWrapper::eventFilter(QObject* object, QEvent* event)
{
    bool result = QToolButton::eventFilter(object, event);

    switch (event->type()) 
    {
        case QEvent::ActionAdded: 
            {
                if (QWidget* parent = parentWidget())
                    QApplication::postEvent(parent, new QResizeEvent(parent->size(), QSize(-1, -1)));
            }
            break;
        case QEvent::Show :
            if (m_reduced && m_ribbonGroup == object)
            {
                m_ribbonGroup->adjustSize();
            }
            break;

        default:
            break;
    }

    return result;
}

bool RibbonGroupWrapper::event(QEvent* event)
{
    switch(event->type())
    {
        case QEvent::LayoutRequest :
               m_realSize = m_ribbonGroup->sizeHint();
            break;
        default:
            break;
    }
    return QToolButton::event(event);
}

void RibbonGroupWrapper::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if (m_reduced) 
    {
        QPainter p(this );
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        QRect rcGroup = opt.rect;
        rcGroup.setBottom(rcGroup.bottom() - 1);
        opt.rect = rcGroup;
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_ReducedGroup, &opt, &p, this);
    }
}

void RibbonGroupWrapper::mousePressEvent(QMouseEvent* event)
{
    if (m_reduced)
    {
        if(ReducedGroupPopupMenu* reducedGroup = qobject_cast<ReducedGroupPopupMenu*>(menu()))
        {
            reducedGroup->setGroup(m_ribbonGroup);
            reducedGroup->setFont(parentWidget()->font());
            m_ribbonGroup->adjustSize();
        }
        QToolButton::mousePressEvent(event);
    }
}

void RibbonGroupWrapper::resizeEvent(QResizeEvent* event)
{
    QToolButton::resizeEvent(event);

    if (!m_reduced && m_ribbonGroup && m_ribbonGroup->isVisible() && event->size() != event->oldSize())
        m_ribbonGroup->setGeometry(rect());
}

/* RibbonGroupOption */
RibbonGroupOption::RibbonGroupOption(QWidget* parent)
    : QToolButton(parent)
{
}

RibbonGroupOption::~RibbonGroupOption()
{
}

QString RibbonGroupOption::text() const
{
    return "";
}

QSize RibbonGroupOption::sizeHint() const
{
    return QSize(15, 14);
}

void RibbonGroupOption::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonOptionButton, &opt, &p, this);
}

