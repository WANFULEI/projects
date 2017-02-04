#include "LayerInterfaceImpl.h"
#include "Component\GlobalInstance.h"
#include "Component\log.h"
#include "QgsRasterLayer.h"
#include "qgsvectorlayer.h"
#include "QgsMapLayerRegistry.h"


LayerInterfaceImpl::LayerInterfaceImpl(void)
{
}


LayerInterfaceImpl::~LayerInterfaceImpl(void)
{
}

int LayerInterfaceImpl::addRasterLayer(QString layerName, QString rasterFilePath)
{
	if(global->getLayerTreeRoot() == 0){
		LOG_ERROR << "m_rootGrou=0, maybe Map2D component not load.";
		return 0;
	}
	if(!QgsRasterLayer::isValidRasterFileName(rasterFilePath)){
		LOG_ERROR << "invalid raster file=" << rasterFilePath.toStdString() << ".";
		return 0;
	}
	if(layerName.isEmpty()){
		LOG_ERROR << "addRasterLayer with a empty layer name is not allowed.";
		return 0;
	}
	QgsRasterLayer *layer = new QgsRasterLayer(rasterFilePath, layerName);
	QgsMapLayerRegistry::instance()->addMapLayer(layer);
	global->getLayerTreeRoot()->insertLayer(0, layer);
	return (int)layer;
}

int LayerInterfaceImpl::addVectorLayer(QString layerName, QString filePath)
{
	if(global->getLayerTreeRoot() == 0){
		LOG_ERROR << "m_rootGrou=0, maybe Map2D component not load.";
		return 0;
	}
	if(!QFile::exists(filePath)){
		LOG_ERROR << QObject::tr("vector file=%1 not exist.").arg(filePath).toStdString();
		return 0;
	}
	if(layerName.isEmpty()){
		LOG_ERROR << "addVectorLayer with a empty layer name is not allowed.";
		return 0;
	}
	QgsVectorLayer *layer = new QgsVectorLayer(filePath, layerName, "ogr", false);
	if(!layer->isValid()){
		QString msg = QObject::tr( "%1 is not a valid or recognized data source" ).arg( filePath );
		LOG_ERROR << msg.toStdString();
		delete layer;
		return 0;
	}
	QgsMapLayerRegistry::instance()->addMapLayer(layer);
	global->getLayerTreeRoot()->insertLayer(0, layer);
	return (int)layer;
}

bool LayerInterfaceImpl::createLayer(QString layerName)
{
	return false;
}

bool LayerInterfaceImpl::removeLayer(QString layerName)
{
	return false;
}

int LayerInterfaceImpl::addElevationLayer(QString layerName, QString filePath)
{
	return addRasterLayer(layerName, filePath);
}
