#include "map2d.h"

map2d::map2d()
{
	QgsProviderRegistry::instance("G:/projects/gis/QGIS/build/output/plugins/RelWithDebInfo");
	//for receive create layer message,add to map curves
	connect(QgsProject::instance(),SIGNAL(readMapLayer( QgsMapLayer *, const QDomElement &)),this,SLOT(slot_readMapLayer( QgsMapLayer *, const QDomElement &)));
	QgsProject::instance()->read(QFileInfo("C:/Users/wq/Desktop/tt2.qgs"));
// 	QDomDocument doc;
// 	QFile file("C:/Users/wq/Desktop/tt2.qgs");
// 	if (!file.open(QIODevice::ReadOnly))
// 	{
// 		//
// 
// 	}
// 	if (!doc.setContent(&file))
// 	{
// 		//
// 	}
// 	readProject(doc);
}

map2d::~map2d()
{

}

void map2d::slot_readMapLayer(QgsMapLayer *mapLayer, const QDomElement &layerNode)
{
	QList<QgsMapLayer *> all_layer = layers();
	all_layer.insert(0,mapLayer);
	QList<QgsMapCanvasLayer> layers2;
	for (int i=0;i<all_layer.size();++i)
	{
		layers2 << all_layer[i];
	}

	setLayerSet(layers2);
}
