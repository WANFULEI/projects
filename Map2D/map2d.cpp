#include "map2d.h"
#include "qgsproject.h"
#include "QgsProviderRegistry.h"
#include "QgsMapLayerRegistry.h"
#include "GlobalInstance.h"
#include <QHboxLayout>
#include "QgsRasterLayer.h"

Map2D::Map2D()
{
	m_mapCanvas = 0;
}

Map2D::~Map2D()
{

}

Q_EXPORT_PLUGIN2(Map2D, Map2D)

void Map2D::initialize(){
	QgsProviderRegistry::instance(qApp->applicationDirPath() + "/../apps/qgis-ltr-dev/plugins");
	m_mapCanvas = new QgsMapCanvas;
	//if(QgsProject::instance()->read(QFileInfo(qApp->applicationDirPath() + "\\..\\data\\tt.qgs"))){
	//	int n = 0;
	//}
	//auto mapLayers = QgsMapLayerRegistry::instance()->mapLayers().values();
	//QList<QgsMapCanvasLayer> canvasLayers;
	//auto iter = mapLayers.begin();
	//while(iter != mapLayers.end()){
	//	canvasLayers << QgsMapCanvasLayer(*iter);
	//	++iter;
	//}
	//m_mapCanvas->setLayerSet(canvasLayers);

	QgsRasterLayer *rasterLayer = new QgsRasterLayer("C:\\OSGeo4W\\data\\NE2_HR_LC_SR_W_DR.tif", "world");
	QgsMapLayerRegistry::instance()->addMapLayer(rasterLayer);
	m_mapCanvas->setLayerSet(QList<QgsMapCanvasLayer>() << QgsMapCanvasLayer(rasterLayer));
	m_mapCanvas->zoomToFullExtent();
	//m_mapCanvas->freeze(false);
	//m_mapCanvas->refresh();
	
	QWidget *w = GlobalInstance::getInstance()->getMainWindow()->findChild<QWidget *>("Map2DWidget");
	if(w) {
		QHBoxLayout *hBox = new QHBoxLayout;
		hBox->addWidget(m_mapCanvas);
		w->setLayout(hBox);
	}else {
		m_mapCanvas->show();
	}
}