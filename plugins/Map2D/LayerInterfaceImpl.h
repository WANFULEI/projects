#pragma once
#include "layerinterface.h"
class LayerInterfaceImpl : public LayerInterface
{
public:
	LayerInterfaceImpl(void);
	~LayerInterfaceImpl(void);

	virtual int addRasterLayer(QString layerName, QString rasterFilePath);
	virtual int addVectorLayer(QString layerName, QString filePath);
	virtual int addElevationLayer(QString layerName, QString filePath);
	virtual bool createLayer(QString layerName);
	virtual bool removeLayer(QString layerName);

};
