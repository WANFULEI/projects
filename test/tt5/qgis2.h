#ifndef QGIS2_H
#define QGIS2_H

#include <QtGui/QMainWindow>
#include "ui_tt5.h"

#define CORE_EXPORT Q_DECL_IMPORT
#define GUI_EXPORT Q_DECL_IMPORT
#define noexcept

#include <qgsapplication.h>
#include <qgsproviderregistry.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsmaptool.h>
#include <QtGui/QVBoxLayout>
#include <qgsmaplayerregistry.h>
#include <QtGui/QToolBar>
#include<qgsmaptoolpan.h>
#include <qgsmaptoolzoom.h>
#include <QgsRasterLayer.h>
#include <QgsLineSymbolLayerV2.h>
#include <QgsFillSymbolLayerV2.h>

class qgis2 : public QMainWindow,private Ui::tt5Class
{
	Q_OBJECT

public:
	qgis2(QWidget *parent = 0, Qt::WFlags flags = 0);
	~qgis2();
	public slots:
		void zoomInMode();
		void zoomOutMode();
		void panMode();
		void addLayer();

private:
	Ui::tt5Class ui;
	QgsMapCanvas *mpMapCanvas;
	QVBoxLayout *mpMapLayout;
	QToolBar * mpMapToolBar;
	QgsMapTool *mpPanTool;
	QgsMapTool *mpZoomInTool;
	QgsMapTool *mpZoomOutTool;
};

#endif // QGIS2_H
