#ifndef MAP2D_H
#define MAP2D_H

#include "map2d_global.h"

#define CORE_EXPORT Q_DECL_IMPORT
#define GUI_EXPORT Q_DECL_IMPORT
#define APP_EXPORT Q_DECL_IMPORT
#define PYTHON_EXPORT Q_DECL_IMPORT
#define noexcept


#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include "../../framecore/framecore/framecore.h"
#include <QDomElement>
#include "../../base/baseset2/baseset2.h"

class MAP2D_EXPORT map2d : public QgsMapCanvas
{
	Q_OBJECT
public:
	map2d();
	~map2d();

private slots:
	void slot_readMapLayer( QgsMapLayer *mapLayer, const QDomElement &layerNode );

};

class map2dcom : public framecore::component , public baseset::instance2<map2dcom>
{
public:
	map2dcom();
private:
	virtual bool initialize();
private:
	map2d * m_map2d;
};

#endif // MAP2D_H
