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
#ifndef QTITAN_RIBBONTASKMENU_DSGN_H
#define QTITAN_RIBBONTASKMENU_DSGN_H

#include <QDesignerTaskMenuExtension>
#include <QtDesigner/QExtensionFactory>


namespace Qtitan
{
    class RibbonBar;

    /* RibbonDsgnTaskMenu */
    class RibbonDsgnTaskMenu : public QObject, public QDesignerTaskMenuExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerTaskMenuExtension)
    public:
        RibbonDsgnTaskMenu(RibbonBar* pRibbon, QObject* parent);
        virtual ~RibbonDsgnTaskMenu();

    public:
        QAction* preferredEditAction() const;
        QList<QAction*> taskActions() const;

    private slots:
        void slotRemoveRibbonBar();
        void slotAddNewPage();
        void slotRemoveCurrentPage();

    private:
        QAction* m_removeRebbonBar;
        QAction* m_addRibbonPage;
        QAction* m_deleteRibbonPage;
        QAction* m_separator;
        RibbonBar* m_pRibbonBar;
    };

    /* RibbonDsgnExFactory */
    class RibbonDsgnTaskMenuExFactory : public QExtensionFactory
    {
        Q_OBJECT
    public:
        RibbonDsgnTaskMenuExFactory(QExtensionManager *parent = 0);

    protected:
        QObject *createExtension(QObject* object, const QString& iid, QObject* parent) const;
    };

}; //namespace Qtitan

#endif // QTITAN_RIBBONTASKMENU_DSGN_H
