/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced RibbonMainWindow for Qt)
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
#include <QPainter>
#include <QLayout>

#include "QtnRibbonBar.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonMainWindow.h"
#ifdef QTITAN_DEMO
#include "QtnDemo.h"
#endif

#include "QtnOfficeFrameHelper.h"

using namespace Qtitan;

namespace Qtitan
{
    /* RibbonMainWindowPrivate */
    class RibbonMainWindowPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(RibbonMainWindow)
    public:
        explicit RibbonMainWindowPrivate();
    public:
        OfficeFrameHelper* frameHelper_;
        bool attrOpaquePaintEvent_;
        bool attrNoSystemBackground_;
    };
};

RibbonMainWindowPrivate::RibbonMainWindowPrivate()
{
    frameHelper_ = Q_NULL;
}


/* RibbonMainWindow */
RibbonMainWindow::RibbonMainWindow(QWidget* parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
{
    QTN_INIT_PRIVATE(RibbonMainWindow);
    setObjectName(QLatin1String("RibbonMainWindow"));
    QTN_D(RibbonMainWindow);
    d.attrOpaquePaintEvent_ = testAttribute(Qt::WA_OpaquePaintEvent);
    d.attrNoSystemBackground_ = testAttribute(Qt::WA_NoSystemBackground);
#ifdef QTITAN_DEMO
    START_QTITAN_DEMO
#endif
}

RibbonMainWindow::~RibbonMainWindow()
{
    QTN_FINI_PRIVATE();
}

RibbonBar* RibbonMainWindow::ribbonBar() const
{
    RibbonBar* ribbonBar = Q_NULL;

    QWidget* menu = menuWidget();
    if (menu)
        ribbonBar = qobject_cast<RibbonBar*>(menu);

    if (!menu && !ribbonBar) 
    {
        RibbonMainWindow* self = const_cast<RibbonMainWindow*>(this);
        ribbonBar = new RibbonBar(self);
        self->setMenuWidget(ribbonBar);
    }
    return ribbonBar;
}

void RibbonMainWindow::setRibbonBar(RibbonBar* ribbonBar)
{
    setMenuWidget(ribbonBar);
}

void RibbonMainWindow::setFrameHelper(OfficeFrameHelper* helper)
{
    QTN_D(RibbonMainWindow);
    d.frameHelper_ = helper;

    if (helper)
    {
        d.attrOpaquePaintEvent_ = testAttribute(Qt::WA_OpaquePaintEvent);
        d.attrNoSystemBackground_ = testAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
    }
    else
    {
        setAttribute(Qt::WA_OpaquePaintEvent, d.attrOpaquePaintEvent_);
        setAttribute(Qt::WA_NoSystemBackground, d.attrNoSystemBackground_);
    }
}

void RibbonMainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonMainWindow);
    if (d.frameHelper_)
    {
        QPainter p(this);
        p.fillRect(0, 0, width(), height(), palette().brush(backgroundRole()));
    }
}

#ifdef Q_OS_WIN
bool RibbonMainWindow::winEvent(MSG* message, long* result)
{
    QTN_D(RibbonMainWindow);
    if (d.frameHelper_)
        return d.frameHelper_->winEvent(message, result);
    return QMainWindow::winEvent(message, result);
}
#endif // Q_OS_WIN
