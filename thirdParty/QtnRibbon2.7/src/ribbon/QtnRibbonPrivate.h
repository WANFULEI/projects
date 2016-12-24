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
#ifndef QTN_RIBBONPRIVATE_H
#define QTN_RIBBONPRIVATE_H

#include <QMenu>
#include <QToolButton>

#include "QtitanDef.h"

class QStyleOptionFrameV2;

namespace Qtitan
{
    class RibbonGroup;
    class ExWidgetWrapperPrivate;

    /* ExWidgetWrapper */
    class ExWidgetWrapper : public QWidget
    {
        Q_OBJECT
    protected:
        ExWidgetWrapper(QWidget* parent, QWidget* widget = Q_NULL);
        virtual ~ExWidgetWrapper();

    public:
        QString labelText() const;
        void setLabelText(const QString& text);

        void setIcon(const QIcon& icon);
        QIcon icon() const;

        void setAlignWidget(bool align);
        bool alignWidget() const;

        void setLengthLabel(int length);

        QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

    public:
        virtual QSize sizeHint() const;

    protected:
        virtual void focusInEvent(QFocusEvent* event);
        virtual void focusOutEvent(QFocusEvent* event);
        virtual void paintEvent(QPaintEvent* event);
        virtual void resizeEvent(QResizeEvent* event);
        virtual void inputMethodEvent(QInputMethodEvent* event);
        virtual bool event(QEvent* event);

    protected:
        void updateGeometries();
        void initStyleOption(QStyleOptionFrameV2* option) const;

    protected:
        virtual bool isReadOnly() const;

    private:
        friend class RibbonGroup;
        QTN_DECLARE_PRIVATE(ExWidgetWrapper)
        Q_DISABLE_COPY(ExWidgetWrapper)
    };


    /* RibbonGroupPopupMenu */
    class ReducedGroupPopupMenu : public QMenu
    {
        Q_OBJECT
    public:
        ReducedGroupPopupMenu(QWidget* parent);
        virtual ~ReducedGroupPopupMenu();

    public:
        void setGroup(RibbonGroup*);
        RibbonGroup* getGroup() const;

    public slots:
        void actionTriggered(QAction* action);

    public:
        virtual QSize sizeHint() const;

    protected:
        virtual void showEvent(QShowEvent* event);
        virtual void hideEvent(QHideEvent* event);
        virtual void paintEvent(QPaintEvent* event);
        virtual void resizeEvent(QResizeEvent* event);

    protected:
        RibbonGroup* m_group;
        QWidget* m_parent;
    };


    /* RibbonGroupWrapper */
    class RibbonGroupWrapper : public QToolButton
    {
        Q_OBJECT
    public:
        RibbonGroupWrapper(QWidget* p);
        virtual ~RibbonGroupWrapper();

    public:
        void setGroup(RibbonGroup* rg);
        RibbonGroup* getGroup() const;

        bool isReducedGroup() const;
        void setReducedGroup(bool);

        void resetReducedGroup();

    public:
        QSize realSize() const;
        QSize reducedSize() const;
        void updateLayout(int* width, bool reset);

        virtual QSize sizeHint() const;
        virtual bool eventFilter(QObject* object, QEvent* event);

    protected:
        virtual bool event(QEvent* event);
        virtual void paintEvent(QPaintEvent* event);
        virtual void mousePressEvent(QMouseEvent* event);
        virtual void resizeEvent(QResizeEvent* event);

    private:
        bool m_reduced;
        QSize m_realSize;
        RibbonGroup* m_ribbonGroup;
    };

    /* RibbonGroupOption */
    class RibbonGroupOption : public QToolButton
    {
        Q_OBJECT
    public:
        RibbonGroupOption(QWidget* parent);
        virtual ~RibbonGroupOption();

    public:
        QString text() const;

    public:
        virtual QSize sizeHint() const;

    protected:
        virtual void paintEvent(QPaintEvent* event);
    };

}; //namespace Qtitan


#endif // QTN_RIBBONPRIVATE_H


