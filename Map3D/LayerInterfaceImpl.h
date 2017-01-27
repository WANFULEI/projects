#pragma once
#include "..\map2d\layerinterface.h"

class LayerInterfaceImpl :
	public LayerInterface
{
public:
	LayerInterfaceImpl(void);
	~LayerInterfaceImpl(void);

	virtual bool addRasterLayer(QString layerName, QString rasterFilePath);
	virtual bool addVectorLayer(QString layerName, QString filePath);
	virtual bool addElevationLayer(QString layerName, QString filePath);
	virtual bool createLayer(QString layerName);
	virtual bool removeLayer(QString layerName);

};

