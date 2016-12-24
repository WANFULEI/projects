/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonSliderPane for Qt)
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
#include <QEvent>
#include <QMouseEvent>
#include <QBasicTimer>
#include <QPainter>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyleOption>

#include "QtnRibbonSliderPane.h"
#include "QtnRibbonStyle.h"

using namespace Qtitan;

namespace Qtitan
{
    /* RibbonSliderButton */
    class RibbonSliderButton : public QPushButton
    {
    public:
        RibbonSliderButton(QWidget* parent, QStyle::PrimitiveElement typeBut = QStyle::PE_IndicatorArrowUp);
        ~RibbonSliderButton();

    protected:
        virtual void  paintEvent(QPaintEvent* event);
        virtual QSize sizeHint() const;

    protected:
        QStyle::PrimitiveElement m_typeBut;

    private:
        Q_DISABLE_COPY(RibbonSliderButton)
    };
}; //namespace Qtitan

/* RibbonSliderButton */
RibbonSliderButton::RibbonSliderButton(QWidget* parent, QStyle::PrimitiveElement typeBut)
  : QPushButton(parent)
{
    m_typeBut = typeBut;
}

RibbonSliderButton::~RibbonSliderButton()
{
}

void RibbonSliderButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.direction = m_typeBut == QStyle::PE_IndicatorArrowUp ? Qt::LeftToRight : Qt::RightToLeft;
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonSliderButton, &option, &painter, this);
}

QSize RibbonSliderButton::sizeHint() const
{
    QSize sz(12, 12);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.direction = m_typeBut == QStyle::PE_IndicatorArrowUp ? Qt::LeftToRight : Qt::RightToLeft;
    sz = style()->sizeFromContents((QStyle::ContentsType)RibbonStyle::CT_RibbonSliderButton, &option, sz, this);
    return sz;
}

namespace Qtitan
{
    /* RibbonSliderPanePrivate */
    class RibbonSliderPanePrivate : QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonSliderPane)
    public:
        explicit RibbonSliderPanePrivate();

    public:
        void initSlider();

    public:
        QSlider* slider_;
        RibbonSliderButton* buttonUp_;
        RibbonSliderButton* buttonDown_;
        QHBoxLayout* layout_;
    };
};//namespace Qtitan

/* RibbonSliderPanePrivate */
RibbonSliderPanePrivate::RibbonSliderPanePrivate()
    : slider_(Q_NULL)
    , buttonUp_(Q_NULL)
    , buttonDown_(Q_NULL)
    , layout_(Q_NULL)
{
}

void RibbonSliderPanePrivate::initSlider()
{
    QTN_P(RibbonSliderPane);
    slider_ = new QSlider(Qt::Horizontal, &p);
    slider_->setTickPosition(QSlider::NoTicks);

    layout_ = new QHBoxLayout();
    layout_->setMargin(0);
    layout_->setSpacing(1);

    layout_->addWidget(slider_);
    p.setLayout(layout_);
}


/* RibbonSliderPane */
RibbonSliderPane::RibbonSliderPane(QWidget* parent)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonSliderPane);
    QTN_D(RibbonSliderPane);
    d.initSlider();
    connect(d.slider_, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(d.slider_, SIGNAL(sliderMoved(int)), this, SIGNAL(sliderMoved(int)));
}

RibbonSliderPane::~RibbonSliderPane()
{
    QTN_FINI_PRIVATE();
}

void RibbonSliderPane::setRange(int min, int max)
{
    QTN_D(RibbonSliderPane);
    d.slider_->setMinimum( min ); // min - Range
    d.slider_->setMaximum( max ); // max - Range
}

void RibbonSliderPane::setScrollButtons(bool on)
{
    QTN_D(RibbonSliderPane);
    if (on)
    {
        d.buttonDown_ = new RibbonSliderButton(this, QStyle::PE_IndicatorArrowDown );
        d.layout_->insertWidget(0, d.buttonDown_, 0, Qt::AlignLeft);
        d.buttonUp_   = new RibbonSliderButton(this, QStyle::PE_IndicatorArrowUp);
        d.layout_->addWidget(d.buttonUp_, 0, Qt::AlignRight);
        connect(d.buttonDown_, SIGNAL(pressed()), this, SLOT(decrement()));
        connect(d.buttonUp_,   SIGNAL(pressed()), this, SLOT(increment()));
    } 
    else 
    {
        disconnect(d.buttonUp_, SIGNAL(pressed()), this, SLOT(increment()));
        disconnect(d.buttonDown_, SIGNAL(pressed()), this, SLOT(decrement()));
        delete d.buttonUp_;
        d.buttonUp_ = NULL;
        delete d.buttonDown_;
        d.buttonDown_ = NULL;
    }
}

void RibbonSliderPane::setSliderPosition(int position)
{
    QTN_D(RibbonSliderPane);
    d.slider_->setSliderPosition(position);
}

int RibbonSliderPane::sliderPosition() const
{
    QTN_D(const RibbonSliderPane);
    return d.slider_->sliderPosition();
}

void RibbonSliderPane::setSingleStep(int nPos)
{
    QTN_D(RibbonSliderPane);
    d.slider_->setSingleStep(nPos);
}

int RibbonSliderPane::singleStep() const
{
    QTN_D(const RibbonSliderPane);
    return d.slider_->singleStep();
}

int RibbonSliderPane::value() const
{
    QTN_D(const RibbonSliderPane);
    return d.slider_->value();
}

void RibbonSliderPane::setValue(int value)
{
    QTN_D(RibbonSliderPane);
    d.slider_->setValue(value);
}

void RibbonSliderPane::increment()
{
    QTN_D(RibbonSliderPane);
    int nPos = d.slider_->sliderPosition() + d.slider_->singleStep();
    if (nPos <= d.slider_->maximum()) 
        d.slider_->setSliderPosition(nPos);
}

void RibbonSliderPane::decrement()
{
    QTN_D(RibbonSliderPane);
    int nPos = d.slider_->sliderPosition() - d.slider_->singleStep();
    if (nPos >= d.slider_->minimum()) 
        d.slider_->setSliderPosition(nPos);
}

