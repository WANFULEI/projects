#pragma once
#include "QObject"

class LayerInterface
{
public:
	LayerInterface(void){}
	virtual ~LayerInterface(void){}

	virtual int addRasterLayer(QString layerName, QString filePath) = 0;
	virtual int addVectorLayer(QString layerName, QString filePath) = 0;
	virtual int addElevationLayer(QString layerName, QString filePath) = 0;
	virtual bool createLayer(QString layerName) = 0;
	virtual bool removeLayer(QString layerName) = 0;
};

Q_DECLARE_INTERFACE(LayerInterface, "LayerInterface")