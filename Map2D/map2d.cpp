#include "map2d.h"
#include "qgsproject.h"
#include "QgsProviderRegistry.h"
#include "QgsMapLayerRegistry.h"
#include "GlobalInstance.h"
#include <QHboxLayout>
#include "QgsRasterLayer.h"
#include "QgsMapToolZoom.h"
#include "QgsMapToolPan.h"
#include "log.h"

Map2D::Map2D()
{
	m_mapCanvas = 0;
}

Map2D::~Map2D()
{
	delete m_mapCanvas;
	//delete m_tools.m_zoomIn;
	//delete m_tools.m_zoomOut;
	//delete m_tools.m_pan;
	//delete m_tools.m_select;
}

Q_EXPORT_PLUGIN2(Map2D, Map2D)

void Map2D::initialize(){
	QgsProviderRegistry::instance(qApp->applicationDirPath() + "/../apps/qgis-ltr-dev/plugins");
	m_mapCanvas = new QgsMapCanvas;
	GlobalInstance::getInstance()->setMap2D(m_mapCanvas);

	QgsRasterLayer *rasterLayer = new QgsRasterLayer("C:\\OSGeo4W\\data\\NE2_HR_LC_SR_W_DR.tif", "world");
	QgsMapLayerRegistry::instance()->addMapLayer(rasterLayer);
	m_mapCanvas->setLayerSet(QList<QgsMapCanvasLayer>() << QgsMapCanvasLayer(rasterLayer));
	m_mapCanvas->zoomToFullExtent();
	//m_mapCanvas->freeze(false);
	//m_mapCanvas->refresh();

	initTools();
	QWidget *w = GlobalInstance::getInstance()->getMainWindow()->findChild<QWidget *>("Map2DWidget");
	if(w) {
		QHBoxLayout *hBox = new QHBoxLayout;
		hBox->addWidget(m_mapCanvas);
		w->setLayout(hBox);
	}else {
		m_mapCanvas->show();
	}
	slotPan();
}

void Map2D::initTools()
{
	m_tools.m_zoomIn = new QgsMapToolZoom( m_mapCanvas, false /* zoomIn */ );
	m_tools.m_zoomOut = new QgsMapToolZoom( m_mapCanvas, true /* zoomOut */ );
	m_tools.m_pan = new QgsMapToolPan( m_mapCanvas );

}

void Map2D::slotPan()
{
	if(m_mapCanvas == 0) return;
	m_mapCanvas->setMapTool(m_tools.m_pan);
}

void Map2D::slotZoomIn()
{
	if(m_mapCanvas == 0) return;
	m_mapCanvas->setMapTool(m_tools.m_zoomIn);
}
void Map2D::slotZoomOut()
{
	if(m_mapCanvas == 0) return;
	m_mapCanvas->setMapTool(m_tools.m_zoomOut);
}

void Map2D::slotReset()
{
	if(m_mapCanvas == 0) return;
	m_mapCanvas->setExtent(m_mapCanvas->fullExtent());
	m_mapCanvas->refresh();
}