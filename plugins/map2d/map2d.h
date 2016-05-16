#ifndef MAP2D_H
#define MAP2D_H

#include "map2d_global.h"

#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include "../../framecore/framecore/framecore.h"
#include <QDomElement>
#include "../../base/baseset2/baseset2.h"


// class map2d : public QgsMapCanvas
// {
// 	Q_OBJECT
// 	friend class map2dcom;
// public:
// 	map2d();
// 	~map2d();
// 
// protected:
// 
// 
// 	
// 
// private slots:
// 	void slot_readMapLayer( QgsMapLayer *mapLayer, const QDomElement &layerNode );
// 
// };

// class layer_manager : public QgsLayerTreeView
// {
// public:
// 	layer_manager(QWidget * parent = 0)
// 		:QgsLayerTreeView(parent){
// 		QgsLayerTreeGroup * group = new QgsLayerTreeGroup("Õº≤„¡–±Ì");
// 		m_model = new QgsLayerTreeModel(group);
// 		setModel(m_model);
// 	}
// 
// private:
// 	QgsLayerTreeModel * m_model;
// };

class QgsLayerTreeView;
class QgsLayerTreeMapCanvasBridge;
class QgsCustomLayerOrderWidget;
class QgsLegendFilterButton;
class QDockWidget;
class QAction;
class QgsAnnotationItem;
class QStatusBar;

class map2dcom : public QObject , public framecore::component , public baseset::instance2<map2dcom>
{
public:
	map2dcom();
	~map2dcom();

	class Tools
	{
	public:

		Tools()
			: mZoomIn( nullptr )
			, mZoomOut( nullptr )
			, mPan( nullptr ){}

		QgsMapTool *mZoomIn;
		QgsMapTool *mZoomOut;
		QgsMapTool *mPan;
	} mMapTools;
private:
	virtual bool initialize();

	virtual void action_triggered(QAction * action);

	void initLayerTreeView();

    /** Opens a qgis project file
      @returns false if unable to open the project
      */
	bool addProject( const QString& projectFile );

	//! clear out any stuff from project
	void closeProject();

	/** Returns all annotation items in the canvas*/
	QList<QgsAnnotationItem *> annotationItems();
	/** Removes annotation items in the canvas*/
	void removeAnnotationItems();
	//! Remove all layers from the map and legend - reimplements same method from qgisappbase
	void removeAllLayers();

	QStatusBar * statusBar() const;

	void createCanvasTools();

private:
	QgsMapCanvas * mMapCanvas;

	QDockWidget * mLayerTreeDock;

	//! Table of contents (legend) for the map
	QgsLayerTreeView * mLayerTreeView;
	//! Helper class that connects layer tree with map canvas
	QgsLayerTreeMapCanvasBridge * mLayerTreeCanvasBridge;
	//! Table of contents (legend) to order layers of the map
	QgsCustomLayerOrderWidget * mMapLayerOrder;

	QAction * mActionFilterLegend;

	QAction * mActionRemoveLayer;

	QgsLegendFilterButton* mLegendExpressionFilterButton;
	QDateTime mProjectLastModified;

// 	//! Overview map canvas
// 	QgsMapOverviewCanvas *mOverviewCanvas;
};

#endif // MAP2D_H
