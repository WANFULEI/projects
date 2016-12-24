#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QPrinter>
#include <QPrintDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QLineEdit>
#include <QRadioButton>
#include <QScrollBar>
#include <QFontComboBox>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QSpinBox>
#include <QMessageBox>
#include <QButtonGroup>

#include <QtnOfficeDefines.h>
#include <QtnRibbonQuickAccessBar.h>
#include <QtnRibbonGallery.h>
#include <QtnOfficePopupMenu.h>

#include "galleryItems.h"
#include "aboutdialog.h"
#include "mainwindow.h"


/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
  : Qtitan::RibbonMainWindow(parent)
{
    m_ribbonStyle = qobject_cast<Qtitan::RibbonStyle*>(qApp->style());
    Q_ASSERT(m_ribbonStyle != Q_NULL);

    m_widgetFontTextColor = Q_NULL;
    m_popupTableGallery = Q_NULL;
    m_popupUndoGallery = Q_NULL;

    setWindowTitle(tr("Qtitan Ribbon Galleries Sample"));

    createMenuFile();
    createQuickAccessBar();
    createGalleryItems();
    createRibbon();
    createStatusBar();

    QAction* actionAbout = ribbonBar()->addAction(QIcon(":/shared/res/about.png"), "About", Qt::ToolButtonIconOnly);
    actionAbout->setToolTip(tr("Display program information, version number and copyright"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    createOptions();

    actionRibbonMinimize = ribbonBar()->addAction(QIcon(":/shared/res/ribbonMinimize.png"), "Minimize the Ribbon", Qt::ToolButtonIconOnly);
    actionRibbonMinimize->setStatusTip(tr("Show only the tab names on the Ribbon"));
    actionRibbonMinimize->setShortcut(tr("Ctrl+F1"));
    connect(actionRibbonMinimize, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect(ribbonBar(), SIGNAL(minimizationChanged(bool)), this, SLOT(minimizationChanged(bool)));

    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
}

MainWindow::~MainWindow()
{
    delete m_itemsFontTextColor;
    delete m_itemsStyles;
    delete m_itemsTable;
    delete m_itemsUndo;
    delete m_itemsShapes;
}

void MainWindow::createMenuFile()
{
    QIcon iconLogo;
    iconLogo.addPixmap(QPixmap(":/shared/res/qtitan.png"));
    iconLogo.addPixmap(QPixmap(":/shared/res/qtitanlogo32x32.png"));
    if (QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File"))) 
    {
        actionFile->setToolTip(tr("Click here to see everything<br />you can do with your<br />document"));

        if (Qtitan::RibbonSystemPopupBar* popupBar = qobject_cast<Qtitan::RibbonSystemPopupBar*>(actionFile->menu()))
        {
            QAction* newFile = popupBar->addAction(QIcon(":/res/new.png"), tr("&New"));
            newFile->setShortcut(tr("Ctrl+N"));
            newFile->setStatusTip(tr("Create a new document"));
            newFile->setToolTip(tr("New"));
            newFile->setEnabled(false);

            QAction* openFile = popupBar->addAction(QIcon(":/res/open.png"), tr("&Open..."));
            openFile->setShortcut(tr("Ctrl+O"));
            openFile->setToolTip(tr("Open"));
            openFile->setStatusTip(tr("Open an existing document"));
            connect(openFile, SIGNAL(triggered()), this, SLOT(open()));

            QAction* saveFile = popupBar->addAction(QIcon(":/res/save.png"), tr("&Save"));
            saveFile->setShortcut(tr("Ctrl+S"));
            saveFile->setToolTip(tr("Save"));
            saveFile->setStatusTip(tr("Save the active document"));
            connect(saveFile, SIGNAL(triggered()), this, SLOT(save()));

            QAction* saveAsFile = popupBar->addAction(tr("Save &As..."));
            saveAsFile->setToolTip(tr("Save As"));
            saveAsFile->setStatusTip(tr("Save the active document with a new name"));
            connect(saveAsFile, SIGNAL(triggered()), this, SLOT(save()));

    #ifndef QT_NO_PRINTER
            popupBar->addSeparator();

            QAction* printFile = popupBar->addAction(QIcon(":/res/print.png"), tr("&Print"));
            printFile->setShortcut(tr("Ctrl+P"));
            printFile->setToolTip(tr("Print"));
            printFile->setStatusTip(tr("Print the active document"));
            connect(printFile, SIGNAL(triggered()), this, SLOT(print()));

            QAction* printPreviewFile = popupBar->addAction(QIcon(":/res/printPreview.png"), tr("Print Pre&view"));
            printPreviewFile->setToolTip(tr("Print Preview"));
            printPreviewFile->setStatusTip(tr("Display full pages"));

            QAction* printSetupFile = popupBar->addAction(tr("P&rint Setup..."));
            printSetupFile->setToolTip(tr("Print Setup"));
            printSetupFile->setStatusTip(tr("Change the printer and printing options"));
            connect(printSetupFile, SIGNAL(triggered()), this, SLOT(printSetup()));
            popupBar->addSeparator();
    #endif // QT_NO_PRINTER

            QAction* actClose = popupBar->addAction(QIcon(":/res/close.png"), tr("&Close"));
            actClose->setShortcut(tr("Ctrl+C"));
            actClose->setStatusTip(tr("Exit"));
            connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
            popupBar->addPopupBarAction(actClose);
            QAction* option = popupBar->addPopupBarAction(tr("Option"));
            option->setEnabled(false);
        }
    }
}

void MainWindow::createQuickAccessBar()
{
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->getQuickAccessBar())
    {
        QAction* action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        QAction* smallButton = quickAccessBar->addAction(QIcon(":/res/smallNew.png"), tr("New"));
        smallButton->setToolTip(tr("Create a new document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallOpen.png"), tr("Open"));
        smallButton->setToolTip(tr("Open an existing document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallSave.png"), tr("Save"));
        smallButton->setToolTip(tr("Save the active document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallUndo.png"), tr("&Undo"));
        smallButton->setShortcut(QKeySequence::Undo);
        smallButton->setEnabled(false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallRedo.png"), tr("&Redo"));
        smallButton->setShortcut(QKeySequence::Redo);
        smallButton->setEnabled(false);

        ribbonBar()->showQuickAccess(true);
    }
}

void MainWindow::createRibbon()
{
    if (Qtitan::RibbonPage* pageGallery = ribbonBar()->addPage( tr("&Galleries 1")))
    {
        if (Qtitan::RibbonGroup* groupPopup = pageGallery->addGroup(tr("Popup Galleries")))
        {
            groupPopup->setOptionButtonVisible();
            QAction* act = groupPopup->getOptionButtonAction();
            act->setToolTip(tr("Popup Galleries"));
            connect(act, SIGNAL(triggered()), this, SLOT(pressButton()));

            groupPopup->setControlsCentering(true);

            OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
            Qtitan::RibbonGallery* popupGallery = new Qtitan::RibbonGallery;
            popupGallery->setBaseSize(QSize(307, 168));
            popupGallery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            popupGallery->setGalleryGroup(m_itemsStyles);
            popup->addWidget(popupGallery);
            popup->addSeparator();
            popup->addAction(tr("Save Style"));
            popup->addAction(tr("Apply Style"));
            groupPopup->addAction(QIcon(":/res/largeStyleButton.png"), tr("Styles"), 
                Qt::ToolButtonTextUnderIcon, popup, QToolButton::InstantPopup);

            popup = OfficePopupMenu::createPopupMenu(this);
            m_popupTableGallery = new Qtitan::RibbonGallery;
            m_popupTableGallery->setBaseSize(QSize(193, 175));
            m_popupTableGallery->setLabelsVisible(true);
            m_popupTableGallery->setScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_popupTableGallery->setGalleryGroup(m_itemsTable);
            popup->addWidget(m_popupTableGallery);
            groupPopup->addAction(QIcon(":/res/largeTable.png"), tr("Table"), 
                Qt::ToolButtonTextUnderIcon, popup, QToolButton::InstantPopup);
            connect(m_popupTableGallery, SIGNAL(itemPressed(RibbonGalleryItem*)), 
                this, SLOT(itemTablePressed(RibbonGalleryItem*)));

            popup = OfficePopupMenu::createPopupMenu(this);
            popupGallery = new Qtitan::RibbonGallery();
            popupGallery->setBaseSize(QSize(173, 145));
            popupGallery->setLabelsVisible(true);
            popupGallery->setScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            popupGallery->setGalleryGroup(m_itemsFontTextColor);
            popup->addWidget(popupGallery);
            groupPopup->addAction(QIcon(":/res/largeColorButton.png"), 
                tr("Color"), Qt::ToolButtonTextUnderIcon, popup, QToolButton::InstantPopup);

            popup = OfficePopupMenu::createPopupMenu(this);
            popup->setShowGripper(false);
            m_popupUndoGallery = new WidgetGalleryUndo();
            m_popupUndoGallery->setGalleryGroup(m_itemsUndo);
            m_popupUndoGallery->setBaseSize(QSize(120, m_itemsUndo->getSize().height() * m_itemsUndo->getItemCount() + 2));
            m_popupUndoGallery->setScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            popup->addWidget(m_popupUndoGallery);

            QLabel* label = new QLabel();
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumWidth(118);
            popup->addWidget(label);
            m_popupUndoGallery->selectedItemChanged();

            m_actionUndo = groupPopup->addAction(QIcon(":/res/largeUndo.png"), 
                tr("Undo"), Qt::ToolButtonTextUnderIcon, popup);
            connect(popup, SIGNAL(aboutToHide()), this, SLOT(aboutToHideUndo()));

            connect(m_popupUndoGallery, SIGNAL(itemPressed(RibbonGalleryItem*)), 
                this, SLOT(itemUndoPressed(RibbonGalleryItem*)));
        }

        if (Qtitan::RibbonGroup* groupInline = pageGallery->addGroup(tr("Inline Galleries")))
        {
            groupInline->setOptionButtonVisible();
            QAction* act = groupInline->getOptionButtonAction();
            act->setToolTip(tr("Inline Galleries"));
            connect(act, SIGNAL(triggered()), this, SLOT(pressButton()));

            groupInline->setControlsCentering(true);

            m_widgetFontTextColor = new Qtitan::RibbonGallery;
            m_widgetFontTextColor->setBaseSize(QSize(190, 60));
            m_widgetFontTextColor->setLabelsVisible(false);
            m_widgetFontTextColor->setScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            m_widgetFontTextColor->setBorderVisible(true);
            m_widgetFontTextColor->setGalleryGroup(m_itemsFontTextColor);
            m_widgetFontTextColor->setCheckedIndex(1);
            act = groupInline->addWidget(m_widgetFontTextColor);
            act->setText(tr("&Color"));
            connect(m_widgetFontTextColor, SIGNAL(itemPressed(RibbonGalleryItem*)), 
                this, SLOT(itemColorPressed(RibbonGalleryItem*)));

            groupInline->addSeparator();

            Qtitan::RibbonGallery* widgetGallery = new Qtitan::RibbonGallery;
            widgetGallery->setBaseSize(QSize(235, 60));
            widgetGallery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            widgetGallery->setLabelsVisible(false);
            widgetGallery->setContentsMargins(0, 1, 0, 1);
            widgetGallery->setBorderVisible(true);
            widgetGallery->setGalleryGroup(m_itemsStyles);

            OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
            widgetGallery->setPopupMenu(popup);

            Qtitan::RibbonGallery* popupGallery = new Qtitan::RibbonGallery;
            popupGallery->setBaseSize(QSize(307, 168));
            popupGallery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            popupGallery->setGalleryGroup(m_itemsStyles);
            popup->addWidget(popupGallery);
            popup->addSeparator();

            // hide page, if enabled minimization signal - released()
            QAction* actSaveStyle = popup->addAction(tr("&Save Style"));
            QObject::connect(actSaveStyle, SIGNAL(triggered()), groupInline, SIGNAL(released()));

            QAction* actApplyStyle = popup->addAction(tr("&Apply Style"));
            QObject::connect(actApplyStyle, SIGNAL(triggered()), groupInline, SIGNAL(released()));
            
            act = groupInline->addWidget(widgetGallery);
            act->setText(tr("&Style"));
        }
    }

    if (Qtitan::RibbonPage* pageGallery = ribbonBar()->addPage( tr("G&alleries 2")))
    {
        if (Qtitan::RibbonGroup* groupPopup = pageGallery->addGroup(tr("Shapes")))
        {
            Qtitan::RibbonGallery* widgetGallery = new Qtitan::RibbonGallery;

            widgetGallery->setBaseSize(QSize(138, 60));
            widgetGallery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            widgetGallery->setBorderVisible(true);
            widgetGallery->setContentsMargins(0, -1, 0, -1);
            widgetGallery->setLabelsVisible(false);
            widgetGallery->setGalleryGroup(m_itemsShapes);

            OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
            widgetGallery->setPopupMenu(popup);
            Qtitan::RibbonGallery* popupGallery = new Qtitan::RibbonGallery;
            popupGallery->setBaseSize(QSize(200, 200));
            popupGallery->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            popupGallery->setGalleryGroup(m_itemsShapes);
            popup->addWidget(popupGallery);

            groupPopup->addWidget(widgetGallery);
        }
    }

    ribbonBar()->setFrameThemeEnabled();
}

void MainWindow::createStatusBar()
{
    Qtitan::RibbonSliderPane* sliderPane = new Qtitan::RibbonSliderPane();
    sliderPane->setScrollButtons(true);
    sliderPane->setRange(0, 100); // Range
    sliderPane->setMinimumWidth(100);
    sliderPane->setSingleStep(10);
    sliderPane->setMaximumWidth(130);
    statusBar()->addPermanentWidget(sliderPane);
}

void MainWindow::createOptions()
{
    Qtitan::RibbonStyle::OptionsStyle styleId = Qtitan::RibbonStyle::OS_OFFICE2007BLUE;
    if (m_ribbonStyle)
        styleId = m_ribbonStyle->getOptionStyle();

    QMenu* menu = ribbonBar()->addMenu(tr("Options"));
    QAction* actionStyle = menu->addAction(tr("Style"));

    QMenu* menuStyle = new QMenu(this);
    QActionGroup* styleActions = new QActionGroup(this);

    QAction* actionBlue = menuStyle->addAction(tr("Office 2007 Blue"));
    actionBlue->setCheckable(true);
    actionBlue->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007BLUE);
    actionBlue->setObjectName("OS_OFFICE2007BLUE");

    QAction* actionBlack = menuStyle->addAction(tr("Office 2007 Black"));
    actionBlack->setObjectName("OS_OFFICE2007BLACK");
    actionBlack->setCheckable(true);
    actionBlack->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007BLACK);

    QAction* actionSilver = menuStyle->addAction(tr("Office 2007 Silver"));
    actionSilver->setObjectName("OS_OFFICE2007SILVER");
    actionSilver->setCheckable(true);
    actionSilver->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007SILVER);

    QAction* actionAqua = menuStyle->addAction(tr("Office 2007 Aqua"));
    actionAqua->setObjectName("OS_OFFICE2007AQUA");
    actionAqua->setCheckable(true);
    actionAqua->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2007AQUA);

    QAction* actionScenic = menuStyle->addAction(tr("Windows 7 Scenic"));
    actionScenic->setObjectName("OS_WINDOWS7SCENIC");
    actionScenic->setCheckable(true);
    actionScenic->setChecked(styleId == Qtitan::RibbonStyle::OS_WINDOWS7SCENIC);

    QAction* action2010Blue = menuStyle->addAction(tr("Office 2010 Blue"));
    action2010Blue->setObjectName("OS_OFFICE2010BLUE");
    action2010Blue->setCheckable(true);
    action2010Blue->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2010BLUE);

    QAction* action2010Silver = menuStyle->addAction(tr("Office 2010 Silver"));
    action2010Silver->setObjectName("OS_OFFICE2010SILVER");
    action2010Silver->setCheckable(true);
    action2010Silver->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2010SILVER);

    QAction* action2010Black = menuStyle->addAction(tr("Office 2010 Black"));
    action2010Black->setObjectName("OS_OFFICE2010BLACK");
    action2010Black->setCheckable(true);
    action2010Black->setChecked(styleId == Qtitan::RibbonStyle::OS_OFFICE2010BLACK);

    styleActions->addAction(actionBlue);
    styleActions->addAction(actionBlack);
    styleActions->addAction(actionSilver);
    styleActions->addAction(actionAqua);
    styleActions->addAction(actionScenic);
    styleActions->addAction(action2010Blue);
    styleActions->addAction(action2010Silver);
    styleActions->addAction(action2010Black);

    actionStyle->setMenu(menuStyle);
    connect(styleActions, SIGNAL(triggered(QAction*)), this, SLOT(options(QAction*)));

    QAction* actionCusomize = menu->addAction(tr("Cusomize..."));
    actionCusomize->setEnabled(false);
}

void MainWindow::createGalleryItems()
{
    m_itemsFontTextColor = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsFontTextColor->setSize(QSize(17, 17));
    m_itemsFontTextColor->addLabel(tr("Theme Colors"));
    GalleryItemFontColor::addThemeColors(m_itemsFontTextColor, 0);
    m_itemsFontTextColor->addLabel(tr("Standard Colors"));
    GalleryItemFontColor::addStandardColors(m_itemsFontTextColor);

    m_itemsStyles = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsStyles->setSize(QSize(72, 56));
    for (int numStyle = 0; numStyle < 16; numStyle++)
    {
        RibbonGalleryItem* item = m_itemsStyles->addItem(numStyle, QPixmap(":/res/galleryStyles.png"), QSize(64, 48));
        QString str = QObject::tr("Style, %1").arg(numStyle + 1);
        item->setToolTip(str);
    }

    //-----------------------------------------------------------
    m_itemsTable = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsTable->setClipItems(false); 
    m_itemsTable->setSize(QSize(19, 19));

    /*RibbonGalleryItem* item =*/ m_itemsTable->addItem(new GalleryItemTableLabel());
    int indexItem;
    for (indexItem= 0; indexItem < 10 * 8; indexItem++)
        m_itemsTable->addItem(new GalleryItemTable());
    //-----------------------------------------------------------

    m_itemsUndo = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsUndo->setSize(QSize(0, 21));
    m_itemsUndo->setClipItems(false);
    for (int i = 0; i < 6; i++)
        m_itemsUndo->addItem(new GalleryItemUndo(QObject::tr("Undo Caption %1").arg(i + 1)));


    m_itemsShapes = Qtitan::RibbonGalleryGroup::createGalleryGroup();
    m_itemsShapes->setSize(QSize(20, 20));

    m_itemsShapes->addLabel(tr("Lines"));
    int indexShape;
    for (indexShape = 0; indexShape < 12; indexShape++)
        m_itemsShapes->addItem(indexShape, QPixmap(":/res/galleryShapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addLabel(tr("Basic Shapes"));
    for (; indexShape < 12 + 32; indexShape++)
        m_itemsShapes->addItem(indexShape, QPixmap(":/res/galleryShapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addLabel(tr("Block Arrows"));
    for (; indexShape < 12 + 32 + 27; indexShape++)
        m_itemsShapes->addItem(indexShape, QPixmap(":/res/galleryShapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addLabel(tr("Flowchart"));
    for (; indexShape < 12 + 32 + 27 + 28; indexShape++)
        m_itemsShapes->addItem(indexShape, QPixmap(":/res/galleryShapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addLabel(tr("Callouts"));
    for (; indexShape < 12 + 32 + 27 + 28 + 20; indexShape++)
        m_itemsShapes->addItem(indexShape, QPixmap(":/res/galleryShapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));

    m_itemsShapes->addLabel(tr("Stars and Banners"));
    for (; indexShape < 12 + 32 + 27 + 28 + 20 + 16; indexShape++)
        m_itemsShapes->addItem(indexShape, QPixmap(":/res/galleryShapes.png"), QSize(20, 20), QColor(0xFF,0,0xFF));
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"));
    if (!fileName.isEmpty()) 
    {
    }
}

bool MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),tr(""));
    if (fileName.isEmpty())
        return false;
    return true;
}

#ifndef QT_NO_PRINTER
void MainWindow::print()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted)
    {
    }
}

void MainWindow::printSetup()
{
}
#endif

void MainWindow::about()
{
    Qtitan::AboutDialog::show(this, tr("About Qtitan Ribbon Controls Sample"), 
        tr("QtitanRibbon"), QLatin1String(QTN_VERSION_RIBBON_STR));
}

void MainWindow::options(QAction* action)
{
    if (m_ribbonStyle)
    {
        Qtitan::RibbonStyle::OptionsStyle styleId = Qtitan::RibbonStyle::OS_OFFICE2007BLUE;
        if (action->objectName() == tr("OS_OFFICE2007BLACK"))
            styleId = Qtitan::RibbonStyle::OS_OFFICE2007BLACK;
        else if (action->objectName() == tr("OS_OFFICE2007SILVER"))
            styleId = Qtitan::RibbonStyle::OS_OFFICE2007SILVER;
        else if (action->objectName() == tr("OS_OFFICE2007AQUA"))
            styleId = Qtitan::RibbonStyle::OS_OFFICE2007AQUA;
        else if (action->objectName() == tr("OS_WINDOWS7SCENIC"))
            styleId = Qtitan::OfficeStyle::OS_WINDOWS7SCENIC;
        else if (action->objectName() == tr("OS_OFFICE2010BLUE"))
            styleId = Qtitan::OfficeStyle::OS_OFFICE2010BLUE;
        else if (action->objectName() == tr("OS_OFFICE2010SILVER"))
            styleId = Qtitan::OfficeStyle::OS_OFFICE2010SILVER;
        else if (action->objectName() == tr("OS_OFFICE2010BLACK"))
            styleId = Qtitan::OfficeStyle::OS_OFFICE2010BLACK;

        m_ribbonStyle->setOptionStyle(styleId);
    }
}

void MainWindow::pressButton()
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), 
        QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Press button."));
    messageBox.exec();
}

void MainWindow::aboutToHideUndo()
{
    m_itemsUndo->clear();

    int count = qrand() % 20 + 3;
    for (int i = 0; i < count; i++)
        m_itemsUndo->addItem(new GalleryItemUndo(QObject::tr("Undo Action %1").arg(i + 1)));

    m_popupUndoGallery->setBaseSize(QSize(120, m_itemsUndo->getSize().height() * count + 2));
}

void MainWindow::itemColorPressed(RibbonGalleryItem* item)
{
    if (m_widgetFontTextColor && m_widgetFontTextColor->getCheckedItem() != item)
    {
        m_widgetFontTextColor->setCheckedItem(item);
        if (GalleryItemFontColor* itemColor = dynamic_cast<GalleryItemFontColor*>(item))
        {
            QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
            QString text = tr("<p>Choose Color.</p>");
            QColor color = itemColor->getColor();
            text += tr("<p>R=%1 G=%2 B=%3</p>").arg(color.red()).arg(color.green()).arg(color.blue());
            text += tr("<p>name=%4</p>").arg(itemColor->caption());
            messageBox.setInformativeText(text);
            messageBox.exec();
        }
    }
}

void MainWindow::itemTablePressed(RibbonGalleryItem* item)
{
    Q_UNUSED(item);
    if (m_popupTableGallery)
    {
        int item = m_popupTableGallery->isItemSelected() ? m_popupTableGallery->getSelectedItem() : -1;

        QRect rcItems = m_popupTableGallery->getItemsRect();
        QSize szItems = m_popupTableGallery->galleryGroup()->getSize();
        int itemsPerRow = rcItems.width() / szItems.width();

        int columns = item < 1 ? 0 : (item - 1) % itemsPerRow + 1;
        int rows = item < 1 ? 0 : (item- 1) / itemsPerRow + 1;

        QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
        messageBox.setInformativeText(tr("Table size %1 x %2").arg(rows).arg(columns));
        messageBox.exec();
    }
}

void MainWindow::itemUndoPressed(RibbonGalleryItem* item)
{
    Q_UNUSED(item);
    if (m_popupUndoGallery)
    {
        QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
        messageBox.setInformativeText(tr("Undo last %1 actions").arg(m_popupUndoGallery->getSelectedItem() + 1));
        messageBox.exec();
    }
}

void MainWindow::maximizeToggle()
{
    ribbonBar()->setMinimized(!ribbonBar()->isMinimized());
}

void MainWindow::minimizationChanged(bool minimized)
{
    actionRibbonMinimize->setChecked(minimized);
    actionRibbonMinimize->setIcon(minimized ? QIcon(":/shared/res/ribbonMaximize.png") :  QIcon(":/shared/res/ribbonMinimize.png"));
}
