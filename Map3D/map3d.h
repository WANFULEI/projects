#ifndef MAP3D_H
#define MAP3D_H

#include "map3d_global.h"
#include "component.h"
#include "../Map2D/LayerInterface.h"
#include <QMap>

namespace osgEarth{
	namespace Util{
		class EarthManipulator;
	}
	class Map;
}
class QgsMapCanvas;
class TiXmlElement;
class QgsMapLayer;
class Map3D : public Component, public LayerInterface
{
	Q_OBJECT
	Q_INTERFACES(LayerInterface)
public:
	Map3D();
	~Map3D();

	void initialize();

private:
	QgsMapCanvas *m_mapCanvas;
	osgEarth::Util::EarthManipulator *m_manip;
	osgEarth::Map *m_map;

private slots:
	void slot_extentsChanged();
	void slotLayersChanged();
	void slotLayersRemoved( const QStringList& layers);

private:
	void removeLayer(QString name);
	void loadMap();
	void loadGroup(TiXmlElement *xmlNode);
	void loadLayer(TiXmlElement *xmlNode);
	QStringList getTypeLayers(QStringList layers, QString type);
	QStringList mapLayers2Layers(QMap<QString, QgsMapLayer *> mapLayers);

	virtual bool addRasterLayer(QString layerName, QString rasterFilePath);

};

#endif // MAP3D_H
