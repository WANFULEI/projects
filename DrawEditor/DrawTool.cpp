#include "DrawTool.h"
#include "GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "QgsMapLayerRegistry.h"
#include "GraphicLayer.h"


DrawTool::DrawTool(void)
{
}


DrawTool::~DrawTool(void)
{
}

GraphicLayer * DrawTool::getActiveLayer() const
{
	QStringList layers = GlobalInstance::getInstance()->getMap2D()->mapSettings().layers();
	for(auto i = layers.begin(); i != layers.end(); ++i){
		GraphicLayer *layer = dynamic_cast<GraphicLayer *>(QgsMapLayerRegistry::instance()->mapLayer(*i));
		if(layer) return layer;
	}
	return 0;
}
