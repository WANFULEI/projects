#include "map2d.h"
#include <qgsapplication.h>
#include <qgsproviderregistry.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsvectorlayer.h>
#include <qgsmaptool.h>
#include <QtGui/QVBoxLayout>
#include <qgsmaplayerregistry.h>
#include <QtGui/QToolBar>
#include <qgsmaptoolpan.h>
#include <qgsmaptoolzoom.h>
#include <QgsRasterLayer.h>
#include <QgsLineSymbolLayerV2.h>
#include <QgsFillSymbolLayerV2.h>
#include <qgsproject.h>
#include "../../framecore/framegui/framegui.h"

extern "C"
{
	MAP2D_EXPORT framecore::component * create_component(const QString & class_name){
		if (class_name == "map2d")
		{
			return map2dcom::get_instance();
		}
		return 0;
	}
};


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


map2dcom::map2dcom()
{
	m_map2d = 0;
}

bool map2dcom::initialize()
{
	if (m_map2d == 0)
	{
		m_map2d = new map2d;
	}
	framegui::center_widget * widget = framegui::framegui::get_instance()->get_type_object<framegui::center_widget>("map2d");
	if (widget)
	{
		widget->set_widget(m_map2d);
		return true;
	}
	delete m_map2d;
	return false;
}


