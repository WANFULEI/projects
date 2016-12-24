/****************************************************************************
**
** Qtitan Framework by Developer Machines
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
#ifndef QTN_RIBBON_STYLE_DSGN_H
#define QTN_RIBBON_STYLE_DSGN_H

#include <QInputContext>
#include <QMainWindow>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerContainerExtension>

#include "QtnRibbonStyle.h"

namespace Qtitan
{

    class DesignerStyleInput: public QInputContext
    {
        Q_OBJECT
    public:
        DesignerStyleInput(QWidget* widget);
        virtual QString identifierName() { return QString(); }
        virtual QString language() { return QString(); }
        virtual void reset() {}
        virtual bool isComposing() const { return false; }
        virtual bool filterEvent(const QEvent* event);

    private slots:
        void removeStyle();

    private:
        QWidget* m_widget;
        QAction* m_actRemoveStyle;
    };

    class RibbonStyleWidget: public QWidget
    {
        Q_OBJECT
    public:
        RibbonStyleWidget(QWidget* parent);
        virtual ~RibbonStyleWidget();
        QStyle* targetStyle() const;
        virtual QSize minimumSizeHint() const;

    protected:
        virtual bool event(QEvent* event);
        virtual void paintEvent(QPaintEvent *);
    private:
        QStyle* m_targetStyle;
        QMainWindow* m_mainWindow;
    };

    class RibbonStyleFake: public RibbonStyleWidget
    {
    public:
        static const QMetaObject staticMetaObject;
        RibbonStyleFake(QWidget* parent);
        virtual ~RibbonStyleFake();
        virtual const QMetaObject *metaObject() const;
    };
    

    class OfficeStyleWidget: public QWidget
    {
        Q_OBJECT
    public:
        OfficeStyleWidget(QWidget* parent);
        virtual ~OfficeStyleWidget();
        QStyle* targetStyle() const;
        virtual QSize minimumSizeHint() const;
    protected:
        virtual bool event(QEvent* event);
        virtual void paintEvent(QPaintEvent *);
    private:
        QStyle* m_targetStyle;
    };


    class OfficeStyleFake: public OfficeStyleWidget
    {
    public:
        static const QMetaObject staticMetaObject;
        OfficeStyleFake(QWidget* parent);
        virtual ~OfficeStyleFake();
        virtual const QMetaObject *metaObject() const;
        int c;
    };

    QWidget* create_fake_ribbon_style_widget(QWidget* parent);
    QWidget* create_fake_office_style_widget(QWidget* parent);


    class ExtensionManagerWrapper: public QExtensionManager
    {
        Q_OBJECT
        Q_INTERFACES(QAbstractExtensionManager)
    public:
        ExtensionManagerWrapper(QExtensionManager *nativeManager = 0);
        virtual ~ExtensionManagerWrapper();
        void setStyleExtentions();
        virtual void registerExtensions(QAbstractExtensionFactory *factory, const QString &iid = QString());
        virtual void unregisterExtensions(QAbstractExtensionFactory *factory, const QString &iid = QString());
        virtual QObject *extension(QObject *object, const QString &iid) const;
        QExtensionManager* nativeManager() const;
    private:
        QExtensionManager* m_nativeManager;
    };

    /* DesignerMainWindowStyleContainer */
    class DesignerMainWindowStyleContainer: public QObject, public QDesignerContainerExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerContainerExtension)
    public:
        DesignerMainWindowStyleContainer(QDesignerContainerExtension* nativeContainer, QMainWindow* mainWindow, QObject* parent);
    public:
        void addWidget(QWidget *widget);
        int count() const;
        int currentIndex() const;
        void insertWidget(int index, QWidget *widget);
        void remove(int index);
        void setCurrentIndex(int index);
        QWidget *widget(int index) const;
        QMainWindow* mainWindow() const;
    private:
        QDesignerContainerExtension* m_nativeContainer;
        QMainWindow* m_mainWindow;
        QWidget* m_styleWidget;
    };


    /* DesignerMainWindowStyleContainerFactory */
    class DesignerMainWindowStyleContainerFactory: public QExtensionFactory
    {
        Q_OBJECT
    public:
        DesignerMainWindowStyleContainerFactory(ExtensionManagerWrapper* parent = Q_NULL);
    protected:
        QObject *createExtension(QObject* object, const QString& iid, QObject* parent) const;
    private:
        QExtensionManager* m_nativeManager;
    };

    //////////////////////////////////////////////////////////////////////////////////////

    class OfficeStyleDsgnPlugin: public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        OfficeStyleDsgnPlugin(QObject *parent = 0);
        bool isContainer() const;
        bool isInitialized() const;
        QIcon icon() const;
        QString domXml() const;
        QString group() const;
        QString includeFile() const;
        QString name() const;
        QString toolTip() const;
        QString whatsThis() const;
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);
    private slots:
        void formWindowAdded(QDesignerFormWindowInterface *formWindow);
        void formWindowRemoved(QDesignerFormWindowInterface *formWindow);
        void widgetManaged(QWidget* widget);
    private:
        bool initialized;
        QDesignerFormEditorInterface* m_core;
    };


    class RibbonStyleDsgnPlugin: public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        RibbonStyleDsgnPlugin(QObject *parent = 0);
        bool isContainer() const;
        bool isInitialized() const;
        QIcon icon() const;
        QString domXml() const;
        QString group() const;
        QString includeFile() const;
        QString name() const;
        QString toolTip() const;
        QString whatsThis() const;
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);
    private slots:
        void formWindowAdded(QDesignerFormWindowInterface *formWindow);
        void formWindowRemoved(QDesignerFormWindowInterface *formWindow);
        void widgetManaged(QWidget* widget);
    private:
        bool initialized;
        QDesignerFormEditorInterface* m_core;
    };

};

#endif //QTN_RIBBON_STYLE_DSGN_H

