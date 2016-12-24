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
#include <QAction>

#include <QtDesigner/QDesignerFormWindowInterface>

#include "QtnRibbonDsgnTaskMenu.h"
#include "QtnCommandDsgn.h"
#include "QtnRibbonBar.h"


using namespace Qtitan;

/* RibbonDsgnTaskMenu */
RibbonDsgnTaskMenu::RibbonDsgnTaskMenu(RibbonBar* pRibbon, QObject *parent)
    : QObject(parent)
{
    m_pRibbonBar = pRibbon;

    m_separator = new QAction(this);
    m_separator->setSeparator(true);

    m_removeRebbonBar = new QAction(tr("Remove Ribbon Bar"), this);
    connect(m_removeRebbonBar, SIGNAL(triggered()), this, SLOT(slotRemoveRibbonBar()));

    m_addRibbonPage = new QAction(tr("Insert page"), this);
    connect(m_addRibbonPage, SIGNAL(triggered()), this, SLOT(slotAddNewPage()));

    m_deleteRibbonPage = new QAction(tr("Delete current page"), this);
    connect(m_deleteRibbonPage, SIGNAL(triggered()), this, SLOT(slotRemoveCurrentPage()));
}

RibbonDsgnTaskMenu::~RibbonDsgnTaskMenu()
{
}

void RibbonDsgnTaskMenu::slotRemoveRibbonBar()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        DeleteRibbonBarCommand* cmd = new DeleteRibbonBarCommand(fw);
        cmd->init(m_pRibbonBar);
        fw->commandHistory()->push(cmd);
    }
}

void RibbonDsgnTaskMenu::slotAddNewPage()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        AddRibbonPageCommand* cmd = new AddRibbonPageCommand(fw);
        cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
        cmd->redo();
        //    fw->commandHistory()->push(cmd);
    }
}

void RibbonDsgnTaskMenu::slotRemoveCurrentPage()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        DeleteRibbonPageCommand* cmd = new DeleteRibbonPageCommand(fw);
        cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
        cmd->redo();
        //    fw->commandHistory()->push(cmd);
    }
}

QAction *RibbonDsgnTaskMenu::preferredEditAction() const
{
    return m_removeRebbonBar;
}

QList<QAction*> RibbonDsgnTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(m_removeRebbonBar);
    list.append(m_separator);
    list.append(m_addRibbonPage);
    if (m_pRibbonBar->getPageCount() > 0)
        list.append(m_deleteRibbonPage);
    return list;
}


/* RibbonDsgnTaskMenuExFactory */
RibbonDsgnTaskMenuExFactory::RibbonDsgnTaskMenuExFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
{
}

QObject* RibbonDsgnTaskMenuExFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    RibbonBar* widget = qobject_cast<RibbonBar*>(object);

    if (widget && (iid == QLatin1String("QDesignerInternalTaskMenuExtension"))) 
        return new RibbonDsgnTaskMenu(widget, parent);
    else
        return 0;
}

