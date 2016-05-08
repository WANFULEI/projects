#ifndef MAP2D_H
#define MAP2D_H

#include "map2d_global.h"

#define CORE_EXPORT Q_DECL_IMPORT
#define GUI_EXPORT Q_DECL_IMPORT
#define APP_EXPORT Q_DECL_IMPORT
#define PYTHON_EXPORT Q_DECL_IMPORT
#define noexcept

#include <qgsapplication.h>
#include <qgsproviderregistry.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsmapcanvas.h>
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

class MAP2D_EXPORT map2d : public QgsMapCanvas
{
	Q_OBJECT
public:
	map2d();
	~map2d();

private:

private slots:
	void slot_readMapLayer( QgsMapLayer *mapLayer, const QDomElement &layerNode );
};

#endif // MAP2D_H
