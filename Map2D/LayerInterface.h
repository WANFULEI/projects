#pragma once
#include "QObject"

class QgsMapLayer;
namespace osgEarth{
	class Layer;
}

class LayerInterface
{
public:
	LayerInterface(void){}
	virtual ~LayerInterface(void){}

	virtual bool addRasterLayer(QString layerName, QString filePath) = 0;
	virtual bool addVectorLayer(QString layerName, QString filePath) = 0;
	virtual bool addElevationLayer(QString layerName, QString filePath) = 0;
	virtual bool createLayer(QString layerName) = 0;
	virtual bool removeLayer(QString layerName) = 0;
};

Q_DECLARE_INTERFACE(LayerInterface, "LayerInterface")