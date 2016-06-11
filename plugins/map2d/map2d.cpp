#include "map2d.h"
#undef max
#undef min
#include <qgsapplication.h>
#include <qgsproviderregistry.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsvectorlayer.h>
#include <qgsmaptool.h>
#include <QtGui/QVBoxLayout>
#include <qgsmaplayerregistry.h>
#include <QtGui/QToolBar>
#include <qgsmaptoolpan.h>
#include <qgsmaptoolzoom.h>
#include <QgsRasterLayer.h>
#include <QgsLineSymbolLayerV2.h>
#include <QgsFillSymbolLayerV2.h>
#include <qgsproject.h>
#include "../../framecore/framegui/framegui.h"
//#include <QgsMapToolIdentifyAction.h>
//#include <QgsMapToolFeatureAction.h>
//#include <QgsMeasureTool.h>
//#include <QgsMapToolFeatureAction.h>
#include <QDockWidget>
#include <qgslayertreeregistrybridge.h>
#include <qgslayertreeviewdefaultactions.h>
#include <QToolButton>
//#include <QgsVisibilityPresets.h>
#include <QgsLayerTreeMapCanvasBridge.h>
#include <QgsCustomLayerOrderWidget.h>
#include <QgsLayerTreeView.h>
#include <QgsLayerTreeModel.h>
#include <QgsLayerTreeGroup.h>
#include <QDockWidget>
#include <QgsLegendFilterButton.h>
#include <QgsAnnotationItem.h>
#include <QMessageBox>
#include <QgsVectorDataProvider.h>
#include <QgsTransactionGroup.h>
#include <qgsstatusbarcoordinateswidget.h>
#include <QgsScaleComboBox.h>
#include <QgsDoubleSpinBox.h>
#include <QgsCoordinateUtils.h>
#include <QCheckBox>
#include <qgsprojectproperties.h>

extern "C"
{
	MAP2D_EXPORT framecore::component * create_component(const QString & class_name){
		if (class_name == "map2d")
		{
			return map2dcom::get_instance();
		}
		return 0;
	}
};


// map2d::map2d()
// {
// 	QgsProviderRegistry::instance("G:/projects/gis/QGIS/build/output/plugins/RelWithDebInfo");
// 	//for receive create layer message,add to map curves
// 	connect(QgsProject::instance(),SIGNAL(readMapLayer( QgsMapLayer *, const QDomElement &)),this,SLOT(slot_readMapLayer( QgsMapLayer *, const QDomElement &)));
// 	QgsProject::instance()->read(QFileInfo("C:/Users/wq/Desktop/tt2.qgs"));
// 
// //	createCanvasTools();
// // 	QDomDocument doc;
// // 	QFile file("C:/Users/wq/Desktop/tt2.qgs");
// // 	if (!file.open(QIODevice::ReadOnly))
// // 	{
// // 		//
// // 
// // 	}
// // 	if (!doc.setContent(&file))
// // 	{
// // 		//
// // 	}
// // 	readProject(doc);
// }
// 
// map2d::~map2d()
// {
// 
// }
// 
// void map2d::slot_readMapLayer(QgsMapLayer *mapLayer, const QDomElement &layerNode)
// {
// 	QList<QgsMapLayer *> all_layer = layers();
// 	all_layer.insert(0,mapLayer);
// 	QList<QgsMapCanvasLayer> layers2;
// 	for (int i=0;i<all_layer.size();++i)
// 	{
// 		layers2 << all_layer[i];
// 	}
// 
// 	setLayerSet(layers2);
// }


map2dcom::map2dcom()
{
	mMapCanvas = 0;
}

map2dcom::~map2dcom()
{

}

bool map2dcom::initialize()
{
	QgsProviderRegistry::instance("G:/projects/gis/QGIS/build/output/plugins/RelWithDebInfo");

	framegui::center_widget * widget = framegui::framegui::get_instance()->get_type_object<framegui::center_widget>("map2d");
	if (widget)
	{
		// "theMapCanvas" used to find this canonical instance later
		mMapCanvas = new QgsMapCanvas( widget, "theMapCanvas" );
// 		connect( mMapCanvas, SIGNAL( messageEmitted( const QString&, const QString&, QgsMessageBar::MessageLevel ) ),
// 			this, SLOT( displayMessage( const QString&, const QString&, QgsMessageBar::MessageLevel ) ) );
		mMapCanvas->setWhatsThis( tr( "Map canvas. This is where raster and vector "
			"layers are displayed when added to the map" ) );

		// set canvas color right away
		QSettings settings;
		int myRed = settings.value( "/qgis/default_canvas_color_red", 255 ).toInt();
		int myGreen = settings.value( "/qgis/default_canvas_color_green", 255 ).toInt();
		int myBlue = settings.value( "/qgis/default_canvas_color_blue", 255 ).toInt();
		mMapCanvas->setCanvasColor( QColor( myRed, myGreen, myBlue ) );

		widget->set_widget(mMapCanvas);
	}
	mLayerTreeDock = framegui::framegui::get_instance()->get_type_object<QDockWidget>("layer_manager");
	if (mLayerTreeDock)
	{
		initLayerTreeView();
	}

	if (mMapCanvas && mLayerTreeView)
	{
		addProject("C:/Users/wq/Desktop/tt2.qgs");
		createCanvasTools();

		mCoordsEdit->setMapCanvas(mMapCanvas);

		connect( mMapCanvas, SIGNAL( scaleChanged( double ) ),
			this, SLOT( showScale( double ) ) );

		connect( mMapCanvas, SIGNAL( rotationChanged( double ) ),
			this, SLOT( showRotation() ) );

		connect( mMapCanvas, SIGNAL( scaleChanged( double ) ),
			this, SLOT( updateMouseCoordinatePrecision() ) );

		connect( mRenderSuppressionCBox, SIGNAL( toggled( bool ) ),
			mMapCanvas, SLOT( setRenderFlag( bool ) ) );
	}
	

	return false;
}

void map2dcom::createCanvasTools()
{
	// create tools
	mMapTools.mZoomIn = new QgsMapToolZoom( mMapCanvas, false /* zoomIn */ );
//	mMapTools.mZoomIn->setAction( mActionZoomIn );
	mMapTools.mZoomOut = new QgsMapToolZoom( mMapCanvas, true /* zoomOut */ );
//	mMapTools.mZoomOut->setAction( mActionZoomOut );
	mMapTools.mPan = new QgsMapToolPan( mMapCanvas );
//	mMapTools.mPan->setAction( mActionPan );
// #ifdef HAVE_TOUCH
// 	mMapTools.mTouch = new QgsMapToolTouch( mMapCanvas );
// 	mMapTools.mTouch->setAction( mActionTouch );
// #endif
// 	mMapTools.mIdentify = new QgsMapToolIdentifyAction( this );
// //	mMapTools.mIdentify->setAction( mActionIdentify );
// // 	connect( mMapTools.mIdentify, SIGNAL( copyToClipboard( QgsFeatureStore & ) ),
// // 		this, SLOT( copyFeatures( QgsFeatureStore & ) ) );
// 	mMapTools.mFeatureAction = new QgsMapToolFeatureAction( this );
// //	mMapTools.mFeatureAction->setAction( mActionFeatureAction );
// 	mMapTools.mMeasureDist = new QgsMeasureTool( this, false /* area */ );
// //	mMapTools.mMeasureDist->setAction( mActionMeasure );
// 	mMapTools.mMeasureArea = new QgsMeasureTool( this, true /* area */ );
//	mMapTools.mMeasureArea->setAction( mActionMeasureArea );
// 	mMapTools.mMeasureAngle = new QgsMapToolMeasureAngle( mMapCanvas );
// 	mMapTools.mMeasureAngle->setAction( mActionMeasureAngle );
// 	mMapTools.mTextAnnotation = new QgsMapToolTextAnnotation( mMapCanvas );
// 	mMapTools.mTextAnnotation->setAction( mActionTextAnnotation );
// 	mMapTools.mFormAnnotation = new QgsMapToolFormAnnotation( mMapCanvas );
// 	mMapTools.mFormAnnotation->setAction( mActionFormAnnotation );
// 	mMapTools.mHtmlAnnotation = new QgsMapToolHtmlAnnotation( mMapCanvas );
// 	mMapTools.mHtmlAnnotation->setAction( mActionHtmlAnnotation );
// 	mMapTools.mSvgAnnotation = new QgsMapToolSvgAnnotation( mMapCanvas );
// 	mMapTools.mSvgAnnotation->setAction( mActionSvgAnnotation );
// 	mMapTools.mAnnotation = new QgsMapToolAnnotation( mMapCanvas );
// 	mMapTools.mAnnotation->setAction( mActionAnnotation );
// 	mMapTools.mAddFeature = new QgsMapToolAddFeature( mMapCanvas );
// 	mMapTools.mAddFeature->setAction( mActionAddFeature );
// 	mMapTools.mCircularStringCurvePoint = new QgsMapToolCircularStringCurvePoint( dynamic_cast<QgsMapToolAddFeature*>( mMapTools.mAddFeature ), mMapCanvas );
// 	mMapTools.mCircularStringCurvePoint->setAction( mActionCircularStringCurvePoint );
// 	mMapTools.mCircularStringRadius = new QgsMapToolCircularStringRadius( dynamic_cast<QgsMapToolAddFeature*>( mMapTools.mAddFeature ), mMapCanvas );
// 	mMapTools.mCircularStringRadius->setAction( mActionCircularStringRadius );
// 	mMapTools.mMoveFeature = new QgsMapToolMoveFeature( mMapCanvas );
// 	mMapTools.mMoveFeature->setAction( mActionMoveFeature );
// 	mMapTools.mRotateFeature = new QgsMapToolRotateFeature( mMapCanvas );
// 	mMapTools.mRotateFeature->setAction( mActionRotateFeature );
// 	//need at least geos 3.3 for OffsetCurve tool
// #if defined(GEOS_VERSION_MAJOR) && defined(GEOS_VERSION_MINOR) && \
// 	((GEOS_VERSION_MAJOR>3) || ((GEOS_VERSION_MAJOR==3) && (GEOS_VERSION_MINOR>=3)))
// 	mMapTools.mOffsetCurve = new QgsMapToolOffsetCurve( mMapCanvas );
// 	mMapTools.mOffsetCurve->setAction( mActionOffsetCurve );
// #else
// 	mAdvancedDigitizeToolBar->removeAction( mActionOffsetCurve );
// 	mEditMenu->removeAction( mActionOffsetCurve );
// 	mMapTools.mOffsetCurve = 0;
// #endif //GEOS_VERSION
// 	mMapTools.mReshapeFeatures = new QgsMapToolReshape( mMapCanvas );
// 	mMapTools.mReshapeFeatures->setAction( mActionReshapeFeatures );
// 	mMapTools.mSplitFeatures = new QgsMapToolSplitFeatures( mMapCanvas );
// 	mMapTools.mSplitFeatures->setAction( mActionSplitFeatures );
// 	mMapTools.mSplitParts = new QgsMapToolSplitParts( mMapCanvas );
// 	mMapTools.mSplitParts->setAction( mActionSplitParts );
// 	mMapTools.mSelectFeatures = new QgsMapToolSelectFeatures( mMapCanvas );
// 	mMapTools.mSelectFeatures->setAction( mActionSelectFeatures );
// 	mMapTools.mSelectPolygon = new QgsMapToolSelectPolygon( mMapCanvas );
// 	mMapTools.mSelectPolygon->setAction( mActionSelectPolygon );
// 	mMapTools.mSelectFreehand = new QgsMapToolSelectFreehand( mMapCanvas );
// 	mMapTools.mSelectFreehand->setAction( mActionSelectFreehand );
// 	mMapTools.mSelectRadius = new QgsMapToolSelectRadius( mMapCanvas );
// 	mMapTools.mSelectRadius->setAction( mActionSelectRadius );
// 	mMapTools.mAddRing = new QgsMapToolAddRing( mMapCanvas );
// 	mMapTools.mAddRing->setAction( mActionAddRing );
// 	mMapTools.mFillRing = new QgsMapToolFillRing( mMapCanvas );
// 	mMapTools.mFillRing->setAction( mActionFillRing );
// 	mMapTools.mAddPart = new QgsMapToolAddPart( mMapCanvas );
// 	mMapTools.mAddPart->setAction( mActionAddPart );
// 	mMapTools.mSimplifyFeature = new QgsMapToolSimplify( mMapCanvas );
// 	mMapTools.mSimplifyFeature->setAction( mActionSimplifyFeature );
// 	mMapTools.mDeleteRing = new QgsMapToolDeleteRing( mMapCanvas );
// 	mMapTools.mDeleteRing->setAction( mActionDeleteRing );
// 	mMapTools.mDeletePart = new QgsMapToolDeletePart( mMapCanvas );
// 	mMapTools.mDeletePart->setAction( mActionDeletePart );
// 	mMapTools.mNodeTool = new QgsMapToolNodeTool( mMapCanvas );
// 	mMapTools.mNodeTool->setAction( mActionNodeTool );
// 	mMapTools.mRotatePointSymbolsTool = new QgsMapToolRotatePointSymbols( mMapCanvas );
// 	mMapTools.mRotatePointSymbolsTool->setAction( mActionRotatePointSymbols );
// 
// 	mMapTools.mPinLabels = new QgsMapToolPinLabels( mMapCanvas );
// 	mMapTools.mPinLabels->setAction( mActionPinLabels );
// 	mMapTools.mShowHideLabels = new QgsMapToolShowHideLabels( mMapCanvas );
// 	mMapTools.mShowHideLabels->setAction( mActionShowHideLabels );
// 	mMapTools.mMoveLabel = new QgsMapToolMoveLabel( mMapCanvas );
// 	mMapTools.mMoveLabel->setAction( mActionMoveLabel );
// 
// 	mMapTools.mRotateLabel = new QgsMapToolRotateLabel( mMapCanvas );
// 	mMapTools.mRotateLabel->setAction( mActionRotateLabel );
// 	mMapTools.mChangeLabelProperties = new QgsMapToolChangeLabelProperties( mMapCanvas );
// 	mMapTools.mChangeLabelProperties->setAction( mActionChangeLabelProperties );
// 	//ensure that non edit tool is initialized or we will get crashes in some situations
// 	mNonEditMapTool = mMapTools.mPan;
}


void map2dcom::action_triggered(QAction * action)
{
	QString s = action->text();
	if (action->objectName() == "选择")
	{
		mMapCanvas->setMapTool(0);
	}
	else if (action->objectName() == "漫游")
	{
		mMapCanvas->setMapTool(mMapTools.mPan);
	}
	else if (action->objectName() == "放大")
	{
		mMapCanvas->setMapTool(mMapTools.mZoomIn);
	}
	else if (action->objectName() == "缩小")
	{
		mMapCanvas->setMapTool(mMapTools.mZoomOut);
	}
	else if (action->objectName() == "复位")
	{
		mMapCanvas->zoomToFullExtent();
	}
	else if (action->objectName() == "test")
	{
		mMapCanvas->setMapTool(new MyMapTool(mMapCanvas));
	}
}

void map2dcom::initLayerTreeView()
{
	mLayerTreeView = new QgsLayerTreeView;
	mLayerTreeView->setObjectName( "theLayerTreeView" ); // "theLayerTreeView" used to find this canonical instance later
	mLayerTreeDock->setWidget(mLayerTreeView);

	mLayerTreeView->setWhatsThis( tr( "Map legend that displays all the layers currently on the map canvas. Click on the check box to turn a layer on or off. Double click on a layer in the legend to customize its appearance and set other properties." ) );

//	mLayerTreeDock = new QDockWidget( tr( "Layers Panel" ), this );
	mLayerTreeDock->setObjectName( "Layers" );
	mLayerTreeDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

	QgsLayerTreeModel* model = new QgsLayerTreeModel( QgsProject::instance()->layerTreeRoot(), this );
	model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
	model->setFlag( QgsLayerTreeModel::AllowNodeRename );
	model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
	model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
	model->setAutoCollapseLegendNodes( 10 );

	mLayerTreeView->setModel( model );
//	mLayerTreeView->setMenuProvider( new QgsAppLayerTreeViewMenuProvider( mLayerTreeView, mMapCanvas ) );

//	setupLayerTreeViewFromSettings();

// 	connect( mLayerTreeView, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( layerTreeViewDoubleClicked( QModelIndex ) ) );
// 	connect( mLayerTreeView, SIGNAL( currentLayerChanged( QgsMapLayer* ) ), this, SLOT( activeLayerChanged( QgsMapLayer* ) ) );
// 	connect( mLayerTreeView->selectionModel(), SIGNAL( currentChanged( QModelIndex, QModelIndex ) ), this, SLOT( updateNewLayerInsertionPoint() ) );
// 	connect( QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL( addedLayersToLayerTree( QList<QgsMapLayer*> ) ),
// 		this, SLOT( autoSelectAddedLayer( QList<QgsMapLayer*> ) ) );

	// add group action
	QAction* actionAddGroup = new QAction( tr( "Add Group" ), this );
	actionAddGroup->setIcon( QgsApplication::getThemeIcon( "/mActionAddGroup.svg" ) );
	actionAddGroup->setToolTip( tr( "Add Group" ) );
	connect( actionAddGroup, SIGNAL( triggered( bool ) ), mLayerTreeView->defaultActions(), SLOT( addGroup() ) );

	// visibility groups tool button
	QToolButton* btnVisibilityPresets = new QToolButton;
	btnVisibilityPresets->setAutoRaise( true );
	btnVisibilityPresets->setToolTip( tr( "Manage Layer Visibility" ) );
	btnVisibilityPresets->setIcon( QgsApplication::getThemeIcon( "/mActionShowAllLayers.svg" ) );
	btnVisibilityPresets->setPopupMode( QToolButton::InstantPopup );
//	btnVisibilityPresets->setMenu( QgsVisibilityPresets::instance()->menu() );

	// filter legend action
	mActionFilterLegend = new QAction( tr( "Filter Legend By Map Content" ), this );
	mActionFilterLegend->setCheckable( true );
	mActionFilterLegend->setToolTip( tr( "Filter Legend By Map Content" ) );
	mActionFilterLegend->setIcon( QgsApplication::getThemeIcon( "/mActionFilter2.svg" ) );
	connect( mActionFilterLegend, SIGNAL( toggled( bool ) ), this, SLOT( updateFilterLegend() ) );

	mLegendExpressionFilterButton = new QgsLegendFilterButton( framegui::framegui::get_instance()->get_main_window() );
	mLegendExpressionFilterButton->setToolTip( tr( "Filter legend by expression" ) );
	connect( mLegendExpressionFilterButton, SIGNAL( toggled( bool ) ), this, SLOT( toggleFilterLegendByExpression( bool ) ) );

	// expand / collapse tool buttons
	QAction* actionExpandAll = new QAction( tr( "Expand All" ), this );
	actionExpandAll->setIcon( QgsApplication::getThemeIcon( "/mActionExpandTree.svg" ) );
	actionExpandAll->setToolTip( tr( "Expand All" ) );
	connect( actionExpandAll, SIGNAL( triggered( bool ) ), mLayerTreeView, SLOT( expandAll() ) );
	QAction* actionCollapseAll = new QAction( tr( "Collapse All" ), this );
	actionCollapseAll->setIcon( QgsApplication::getThemeIcon( "/mActionCollapseTree.svg" ) );
	actionCollapseAll->setToolTip( tr( "Collapse All" ) );
	connect( actionCollapseAll, SIGNAL( triggered( bool ) ), mLayerTreeView, SLOT( collapseAll() ) );

	QToolBar* toolbar = new QToolBar();
	toolbar->setIconSize( QSize( 16, 16 ) );
	toolbar->addAction( actionAddGroup );
	toolbar->addWidget( btnVisibilityPresets );
	toolbar->addAction( mActionFilterLegend );
	toolbar->addWidget( mLegendExpressionFilterButton );
	toolbar->addAction( actionExpandAll );
	toolbar->addAction( actionCollapseAll );

	mActionRemoveLayer = new QAction(QIcon(""),"remove",0);

	connect( mActionRemoveLayer, SIGNAL( triggered() ), this, SLOT( removeLayer() ) );
	toolbar->addAction( mActionRemoveLayer );

	QVBoxLayout* vboxLayout = new QVBoxLayout;
	vboxLayout->setMargin( 0 );
	vboxLayout->addWidget( toolbar );
	vboxLayout->addWidget( mLayerTreeView );

	QWidget* w = new QWidget;
	w->setLayout( vboxLayout );
	mLayerTreeDock->setWidget( w );
//	addDockWidget( Qt::LeftDockWidgetArea, mLayerTreeDock );

	mLayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge( QgsProject::instance()->layerTreeRoot(), mMapCanvas, this );
	connect( QgsProject::instance(), SIGNAL( writeProject( QDomDocument& ) ), mLayerTreeCanvasBridge, SLOT( writeProject( QDomDocument& ) ) );
	connect( QgsProject::instance(), SIGNAL( readProject( QDomDocument ) ), mLayerTreeCanvasBridge, SLOT( readProject( QDomDocument ) ) );

	bool otfTransformAutoEnable = QSettings().value( "/Projections/otfTransformAutoEnable", true ).toBool();
	mLayerTreeCanvasBridge->setAutoEnableCrsTransform( otfTransformAutoEnable );

// 	mMapLayerOrder = new QgsCustomLayerOrderWidget( mLayerTreeCanvasBridge, this );
// 	mMapLayerOrder->setObjectName( "theMapLayerOrder" );
// 
// 	mMapLayerOrder->setWhatsThis( tr( "Map layer list that displays all layers in drawing order." ) );
// 	mLayerOrderDock = new QDockWidget( tr( "Layer Order Panel" ), this );
// 	mLayerOrderDock->setObjectName( "LayerOrder" );
// 	mLayerOrderDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
// 
// 	mLayerOrderDock->setWidget( mMapLayerOrder );
// 	addDockWidget( Qt::LeftDockWidgetArea, mLayerOrderDock );
// 	mLayerOrderDock->hide();

	connect( mMapCanvas, SIGNAL( mapCanvasRefreshed() ), this, SLOT( updateFilterLegend() ) );
}

/**
  adds a saved project to qgis, usually called on startup by specifying a
  project file on the command line
  */
bool map2dcom::addProject( const QString& projectFile )
{
  QFileInfo pfi( projectFile );
  statusBar()->showMessage( tr( "Loading project: %1" ).arg( pfi.fileName() ) );
  qApp->processEvents();

  QApplication::setOverrideCursor( Qt::WaitCursor );

  // close the previous opened project if any
  closeProject();

  if ( !QgsProject::instance()->read( projectFile ) )
  {
    QString backupFile = projectFile + "~";
    QString loadBackupPrompt;
    QMessageBox::StandardButtons buttons;
    if ( QFile( backupFile ).exists() )
    {
      loadBackupPrompt = "\n\n" + tr( "Do you want to open the backup file\n%1\ninstead?" ).arg( backupFile );
      buttons |= QMessageBox::Yes;
      buttons |= QMessageBox::No;
    }
    else
    {
      buttons |= QMessageBox::Ok;
    }
    QApplication::restoreOverrideCursor();
    statusBar()->clearMessage();

    int r = QMessageBox::critical( framegui::framegui::get_instance()->get_main_window(),
                                   tr( "Unable to open project" ),
                                   QgsProject::instance()->error() + loadBackupPrompt,
                                   buttons );

    if ( QMessageBox::Yes == r && addProject( backupFile ) )
    {
      // We loaded data from the backup file, but we pretend to work on the original project file.
      QgsProject::instance()->setFileName( projectFile );
      QgsProject::instance()->setDirty( true );
      mProjectLastModified = pfi.lastModified();
      return true;
    }

    mMapCanvas->freeze( false );
    mMapCanvas->refresh();
    return false;
  }

  mProjectLastModified = pfi.lastModified();

//  setTitleBarText_( *this );
  int  myRedInt = QgsProject::instance()->readNumEntry( "Gui", "/CanvasColorRedPart", 255 );
  int  myGreenInt = QgsProject::instance()->readNumEntry( "Gui", "/CanvasColorGreenPart", 255 );
  int  myBlueInt = QgsProject::instance()->readNumEntry( "Gui", "/CanvasColorBluePart", 255 );
  QColor myColor = QColor( myRedInt, myGreenInt, myBlueInt );
  mMapCanvas->setCanvasColor( myColor ); //this is fill color before rendering starts
//  mOverviewCanvas->setBackgroundColor( myColor );

  QgsDebugMsg( "Canvas background color restored..." );
  int myAlphaInt = QgsProject::instance()->readNumEntry( "Gui", "/SelectionColorAlphaPart", 255 );
  myRedInt = QgsProject::instance()->readNumEntry( "Gui", "/SelectionColorRedPart", 255 );
  myGreenInt = QgsProject::instance()->readNumEntry( "Gui", "/SelectionColorGreenPart", 255 );
  myBlueInt = QgsProject::instance()->readNumEntry( "Gui", "/SelectionColorBluePart", 0 );
  myColor = QColor( myRedInt, myGreenInt, myBlueInt, myAlphaInt );
  mMapCanvas->setSelectionColor( myColor ); //this is selection color before rendering starts

  //load project scales
  bool projectScales = QgsProject::instance()->readBoolEntry( "Scales", "/useProjectScales" );
  if ( projectScales )
  {
//    mScaleEdit->updateScales( QgsProject::instance()->readListEntry( "Scales", "/ScalesList" ) );
  }

  mMapCanvas->updateScale();
  QgsDebugMsg( "Scale restored..." );

  mActionFilterLegend->setChecked( QgsProject::instance()->readBoolEntry( "Legend", "filterByMap" ) );

  QSettings settings;

  // does the project have any macros?
//   if ( mPythonUtils && mPythonUtils->isEnabled() )
//   {
//     if ( !QgsProject::instance()->readEntry( "Macros", "/pythonCode", QString::null ).isEmpty() )
//     {
//       int enableMacros = settings.value( "/qgis/enableMacros", 1 ).toInt();
//       // 0 = never, 1 = ask, 2 = just for this session, 3 = always
// 
//       if ( enableMacros == 3 || enableMacros == 2 )
//       {
//         enableProjectMacros();
//       }
//       else if ( enableMacros == 1 ) // ask
//       {
//         // create the notification widget for macros
// 
// 
//         QToolButton *btnEnableMacros = new QToolButton();
//         btnEnableMacros->setText( tr( "Enable macros" ) );
//         btnEnableMacros->setStyleSheet( "background-color: rgba(255, 255, 255, 0); color: black; text-decoration: underline;" );
//         btnEnableMacros->setCursor( Qt::PointingHandCursor );
//         btnEnableMacros->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );
//         connect( btnEnableMacros, SIGNAL( clicked() ), mInfoBar, SLOT( popWidget() ) );
//         connect( btnEnableMacros, SIGNAL( clicked() ), this, SLOT( enableProjectMacros() ) );
// 
//         QgsMessageBarItem *macroMsg = new QgsMessageBarItem(
//           tr( "Security warning" ),
//           tr( "project macros have been disabled." ),
//           btnEnableMacros,
//           QgsMessageBar::WARNING,
//           0,
//           mInfoBar );
//         // display the macros notification widget
//         mInfoBar->pushItem( macroMsg );
//       }
//     }
//   }

//  emit projectRead(); // let plug-ins know that we've read in a new
  // project so that they can check any project
  // specific plug-in state

  // add this to the list of recently used project files
//  saveRecentProjectPath( projectFile, false );

  QApplication::restoreOverrideCursor();

  mMapCanvas->freeze( false );
  mMapCanvas->refresh();

  statusBar()->showMessage( tr( "Project loaded" ), 3000 );
  return true;
} // QgisApp::addProject(QString projectFile)

void map2dcom::closeProject()
{
	// unload the project macros before changing anything
// 	if ( mTrustedMacros )
// 	{
// 		QgsPythonRunner::run( "qgis.utils.unloadProjectMacros();" );
// 	}
// 
// 	mTrustedMacros = false;

	mLegendExpressionFilterButton->setExpressionText( "" );
	mLegendExpressionFilterButton->setChecked( false );
	mActionFilterLegend->setChecked( false );

//	deletePrintComposers();
	removeAnnotationItems();
	// clear out any stuff from project
	mMapCanvas->freeze( true );
	QList<QgsMapCanvasLayer> emptyList;
	mMapCanvas->setLayerSet( emptyList );
	mMapCanvas->clearCache();
	removeAllLayers();
}

QList<QgsAnnotationItem*> map2dcom::annotationItems()
{
	QList<QgsAnnotationItem*> itemList;

	if ( !mMapCanvas )
	{
		return itemList;
	}

	if ( mMapCanvas )
	{
		QList<QGraphicsItem*> graphicsItems = mMapCanvas->items();
		QList<QGraphicsItem*>::iterator gIt = graphicsItems.begin();
		for ( ; gIt != graphicsItems.end(); ++gIt )
		{
			QgsAnnotationItem* currentItem = dynamic_cast<QgsAnnotationItem*>( *gIt );
			if ( currentItem )
			{
				itemList.push_back( currentItem );
			}
		}
	}
	return itemList;
}

void map2dcom::removeAnnotationItems()
{
	if ( !mMapCanvas )
	{
		return;
	}
	QGraphicsScene* scene = mMapCanvas->scene();
	if ( !scene )
	{
		return;
	}
	QList<QgsAnnotationItem*> itemList = annotationItems();
	QList<QgsAnnotationItem*>::iterator itemIt = itemList.begin();
	for ( ; itemIt != itemList.end(); ++itemIt )
	{
		if ( *itemIt )
		{
			scene->removeItem( *itemIt );
			delete *itemIt;
		}
	}
}

void map2dcom::removeAllLayers()
{
	QgsMapLayerRegistry::instance()->removeAllMapLayers();
}

QStatusBar * map2dcom::statusBar() const
{
	return framegui::framegui::get_instance()->status_bar();
}

void map2dcom::removeLayer()
{
	if ( !mLayerTreeView )
	{
		return;
	}

	Q_FOREACH ( QgsMapLayer * layer, mLayerTreeView->selectedLayers() )
	{
		QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer*>( layer );
		if ( vlayer && vlayer->isEditable() && !toggleEditing( vlayer, true ) )
			return;
	}

	QList<QgsLayerTreeNode*> selectedNodes = mLayerTreeView->selectedNodes( true );

// 	//validate selection
// 	if ( selectedNodes.isEmpty() )
// 	{
// 		messageBar()->pushMessage( tr( "No legend entries selected" ),
// 			tr( "Select the layers and groups you want to remove in the legend." ),
// 			QgsMessageBar::INFO, messageTimeout() );
// 		return;
// 	}

	bool promptConfirmation = QSettings().value( "qgis/askToDeleteLayers", true ).toBool();
	bool shiftHeld = QApplication::queryKeyboardModifiers().testFlag( Qt::ShiftModifier );
	//display a warning
	if ( !shiftHeld && promptConfirmation && QMessageBox::warning( framegui::framegui::get_instance()->get_main_window(), tr( "Remove layers and groups" ), tr( "Remove %n legend entries?", "number of legend items to remove", selectedNodes.count() ), QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
	{
		return;
	}

	Q_FOREACH ( QgsLayerTreeNode* node, selectedNodes )
	{
		QgsLayerTreeGroup* parentGroup = qobject_cast<QgsLayerTreeGroup*>( node->parent() );
		if ( parentGroup )
			parentGroup->removeChildNode( node );
	}

	showStatusMessage( tr( "%n legend entries removed.", "number of removed legend entries", selectedNodes.count() ) );

	mMapCanvas->refresh();
}

bool map2dcom::toggleEditing( QgsMapLayer *layer, bool allowCancel )
{
	QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
	if ( !vlayer )
	{
		return false;
	}

	bool res = true;

	QString connString = QgsDataSourceURI( vlayer->source() ).connectionInfo();
	QString key = vlayer->providerType();

	QMap< QPair< QString, QString>, QgsTransactionGroup*> transactionGroups = QgsProject::instance()->transactionGroups();
	QMap< QPair< QString, QString>, QgsTransactionGroup*>::iterator tIt = transactionGroups .find( qMakePair( key, connString ) );
	QgsTransactionGroup* tg = ( tIt != transactionGroups.end() ? tIt.value() : nullptr );

	bool isModified = false;

	// Assume changes if: a) the layer reports modifications or b) its transaction group was modified
	if ( vlayer->isModified() || ( tg && tg->layers().contains( vlayer ) && tg->modified() ) )
		isModified  = true;

	if ( !vlayer->isEditable() && !vlayer->readOnly() )
	{
		if ( !( vlayer->dataProvider()->capabilities() & QgsVectorDataProvider::EditingCapabilities ) )
		{
// 			mActionToggleEditing->setChecked( false );
// 			mActionToggleEditing->setEnabled( false );
// 			messageBar()->pushMessage( tr( "Start editing failed" ),
// 				tr( "Provider cannot be opened for editing" ),
// 				QgsMessageBar::INFO, messageTimeout() );
			return false;
		}

		vlayer->startEditing();

		QSettings settings;
		QString markerType = settings.value( "/qgis/digitizing/marker_style", "Cross" ).toString();
		bool markSelectedOnly = settings.value( "/qgis/digitizing/marker_only_for_selected", false ).toBool();

		// redraw only if markers will be drawn
		if (( !markSelectedOnly || vlayer->selectedFeatureCount() > 0 ) &&
			( markerType == "Cross" || markerType == "SemiTransparentCircle" ) )
		{
			vlayer->triggerRepaint();
		}
	}
	else if ( isModified )
	{
		QMessageBox::StandardButtons buttons = QMessageBox::Save | QMessageBox::Discard;
		if ( allowCancel )
			buttons |= QMessageBox::Cancel;

		switch ( QMessageBox::information( nullptr,
			tr( "Stop editing" ),
			tr( "Do you want to save the changes to layer %1?" ).arg( vlayer->name() ),
			buttons ) )
		{
		case QMessageBox::Cancel:
			res = false;
			break;

		case QMessageBox::Save:
			QApplication::setOverrideCursor( Qt::WaitCursor );

			if ( !vlayer->commitChanges() )
			{
//				commitError( vlayer );
				// Leave the in-memory editing state alone,
				// to give the user a chance to enter different values
				// and try the commit again later
				res = false;
			}

			vlayer->triggerRepaint();

			QApplication::restoreOverrideCursor();
			break;

		case QMessageBox::Discard:
			QApplication::setOverrideCursor( Qt::WaitCursor );

			mMapCanvas->freeze( true );
			if ( !vlayer->rollBack() )
			{
// 				messageBar()->pushMessage( tr( "Error" ),
// 					tr( "Problems during roll back" ),
// 					QgsMessageBar::CRITICAL );
				res = false;
			}
			mMapCanvas->freeze( false );

			vlayer->triggerRepaint();

			QApplication::restoreOverrideCursor();
			break;

		default:
			break;
		}
	}
	else //layer not modified
	{
		mMapCanvas->freeze( true );
		vlayer->rollBack();
		mMapCanvas->freeze( false );
		res = true;
		vlayer->triggerRepaint();
	}

	if ( !res && layer == activeLayer() )
	{
		// while also called when layer sends editingStarted/editingStopped signals,
		// this ensures correct restoring of gui state if toggling was canceled
		// or layer commit/rollback functions failed
//		activateDeactivateLayerRelatedActions( layer );
	}

	return res;
}

/** Get a pointer to the currently selected map layer */
QgsMapLayer *map2dcom::activeLayer()
{
	return mLayerTreeView ? mLayerTreeView->currentLayer() : nullptr;
}

void map2dcom::showStatusMessage( const QString& theMessage )
{
	statusBar()->showMessage( theMessage );
}

void map2dcom::on_create_control(const QString & id,QWidget * widget)
{
	if (id == "map_render")
	{
		// render suppression status bar widget
		mRenderSuppressionCBox = dynamic_cast<QCheckBox *>(widget);
		mRenderSuppressionCBox->setObjectName( "mRenderSuppressionCBox" );
		mRenderSuppressionCBox->setChecked( true );
		//mRenderSuppressionCBox->setFont( myFont );
		mRenderSuppressionCBox->setWhatsThis( tr( "When checked, the map layers "
			"are rendered in response to map navigation commands and other "
			"events. When not checked, no rendering is done. This allows you "
			"to add a large number of layers and symbolize them before rendering." ) );
		mRenderSuppressionCBox->setToolTip( tr( "Toggle map rendering" ) );
	}
	else if (id == "map_properties")
	{
		// On the fly projection status bar icon
		// Changed this to a tool button since a QPushButton is
		// sculpted on OS X and the icon is never displayed [gsherman]
		mOnTheFlyProjectionStatusButton = dynamic_cast<QToolButton *>(widget);
		mOnTheFlyProjectionStatusButton->setAutoRaise( true );
		mOnTheFlyProjectionStatusButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		mOnTheFlyProjectionStatusButton->setObjectName( "mOntheFlyProjectionStatusButton" );
		// Maintain uniform widget height in status bar by setting button height same as labels
		// For Qt/Mac 3.3, the default toolbutton height is 30 and labels were expanding to match
		//mOnTheFlyProjectionStatusButton->setMaximumHeight( mScaleLabel->height() );
		mOnTheFlyProjectionStatusButton->setIcon( QgsApplication::getThemeIcon( "mIconProjectionEnabled.png" ) );
		mOnTheFlyProjectionStatusButton->setWhatsThis( tr( "This icon shows whether "
			"on the fly coordinate reference system transformation is enabled or not. "
			"Click the icon to bring up "
			"the project properties dialog to alter this behaviour." ) );
		mOnTheFlyProjectionStatusButton->setToolTip( tr( "CRS status - Click "
			"to open coordinate reference system dialog" ) );
		connect( mOnTheFlyProjectionStatusButton, SIGNAL( clicked() ),
			this, SLOT( projectPropertiesProjections() ) );//bring up the project props dialog when clicked
	}
}

QWidget * map2dcom::create_control(const QString & control_id)
{
	if (control_id == "map_coordinate")
	{
		mCoordsEdit = new QgsStatusBarCoordinatesWidget(0);
		
		mCoordsEdit->setMouseCoordinatesPrecision(6);

		mCoordsEdit->getLable()->setText(tr("坐标："));
		
		return mCoordsEdit;
	}
	else if (control_id == "map_scale")
	{
		mScaleEdit = new QgsScaleComboBox( statusBar() );
		mScaleEdit->setObjectName( "mScaleEdit" );
		//mScaleEdit->setFont( myFont );
		// seems setFont() change font only for popup not for line edit,
		// so we need to set font for it separately
		//mScaleEdit->lineEdit()->setFont( myFont );
		mScaleEdit->setMinimumWidth( 120 );
		mScaleEdit->setContentsMargins( 0, 0, 0, 0 );
		mScaleEdit->setWhatsThis( tr( "Displays the current map scale" ) );
		mScaleEdit->setToolTip( tr( "Current map scale (formatted as x:y)" ) );

		connect( mScaleEdit, SIGNAL( scaleChanged( double ) ), this, SLOT( userScale() ) );

		return mScaleEdit;
	}
	else if (control_id == "map_rotation")
	{
		mRotationEdit = new QgsDoubleSpinBox(0);
		mRotationEdit->setObjectName( "mRotationEdit" );
		mRotationEdit->setClearValue( 0.0 );
		mRotationEdit->setKeyboardTracking( false );
		mRotationEdit->setMaximumWidth( 120 );
		mRotationEdit->setDecimals( 1 );
		mRotationEdit->setRange( -180.0, 180.0 );
		mRotationEdit->setWrapping( true );
		mRotationEdit->setSingleStep( 5.0 );
		//mRotationEdit->setFont( myFont );
		mRotationEdit->setWhatsThis( tr( "Shows the current map clockwise rotation "
			"in degrees. It also allows editing to set "
			"the rotation" ) );
		mRotationEdit->setToolTip( tr( "Current clockwise map rotation in degrees" ) );
		connect( mRotationEdit, SIGNAL( valueChanged( double ) ), this, SLOT( userRotation() ) );

		showRotation();
		return mRotationEdit;
	}
	return 0;
}

void map2dcom::userScale()
{
	// Why has MapCanvas the scale inverted?
	mMapCanvas->zoomScale( 1.0 / mScaleEdit->scale() );
}

void map2dcom::showScale( double theScale )
{
	// Why has MapCanvas the scale inverted?
	mScaleEdit->setScale( 1.0 / theScale );

	// Not sure if the lines below do anything meaningful /Homann
	if ( mScaleEdit->width() > mScaleEdit->minimumWidth() )
	{
		mScaleEdit->setMinimumWidth( mScaleEdit->width() );
	}
}

void map2dcom::userRotation()
{
	if (mMapCanvas == 0)
	{
		return;
	}
	double degrees = mRotationEdit->value();
	mMapCanvas->setRotation( degrees );
	mMapCanvas->refresh();
}

void map2dcom::showRotation()
{
	if (mMapCanvas == 0)
	{
		return;
	}
	// update the statusbar with the current rotation.
	double myrotation = mMapCanvas->rotation();
	mRotationEdit->setValue( myrotation );
} // QgisApp::showRotation

void map2dcom::updateMouseCoordinatePrecision()
{
	mCoordsEdit->setMouseCoordinatesPrecision( QgsCoordinateUtils::calculateCoordinatePrecision( mapCanvas()->mapUnitsPerPixel(), mapCanvas()->mapSettings().destinationCrs() ) );
}

QgsMapCanvas *map2dcom::mapCanvas()
{
	Q_ASSERT( mMapCanvas );
	return mMapCanvas;
}

void map2dcom::projectPropertiesProjections()
{
	// Driver to display the project props dialog and switch to the
	// projections tab
	mShowProjectionTab = true;
	projectProperties();
}

void map2dcom::projectProperties()
{
	/* Display the property sheet for the Project */
	// set wait cursor since construction of the project properties
	// dialog results in the construction of the spatial reference
	// system QMap
	QApplication::setOverrideCursor( Qt::WaitCursor );
	QgsProjectProperties *pp = new QgsProjectProperties( mMapCanvas , framegui::framegui::get_instance()->get_main_window() );
	// if called from the status bar, show the projection tab
	if ( mShowProjectionTab )
	{
		pp->showProjectionsTab();
		mShowProjectionTab = false;
	}
	qApp->processEvents();
	// Be told if the mouse display precision may have changed by the user
	// changing things in the project properties dialog box
	connect( pp, SIGNAL( displayPrecisionChanged() ), this,
		SLOT( updateMouseCoordinatePrecision() ) );

	connect( pp, SIGNAL( scalesChanged( const QStringList & ) ), mScaleEdit,
		SLOT( updateScales( const QStringList & ) ) );
	QApplication::restoreOverrideCursor();

	//pass any refresh signals off to canvases
	// Line below was commented out by wonder three years ago (r4949).
	// It is needed to refresh scale bar after changing display units.
	connect( pp, SIGNAL( refresh() ), mMapCanvas, SLOT( refresh() ) );

	// Display the modal dialog box.
	pp->exec();

//	qobject_cast<QgsMeasureTool*>( mMapTools.mMeasureDist )->updateSettings();
//	qobject_cast<QgsMeasureTool*>( mMapTools.mMeasureArea )->updateSettings();
//	qobject_cast<QgsMapToolMeasureAngle*>( mMapTools.mMeasureAngle )->updateSettings();

	// Set the window title.
//	setTitleBarText_( *this );

	// delete the property sheet object
	delete pp;
} // QgisApp::projectProperties

MyMapTool::MyMapTool(QgsMapCanvas * map)
	:QgsMapTool(map)
{

}

void MyMapTool::canvasMoveEvent(QgsMapMouseEvent* e)
{

}

void MyMapTool::canvasPressEvent(QgsMapMouseEvent* e)
{
}

void MyMapTool::canvasReleaseEvent(QgsMapMouseEvent* e)
{
}
