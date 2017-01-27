#ifndef MAP3D_H
#define MAP3D_H

#include "component/component.h"
#include "LayerInterfaceImpl.h"
#include "QMap"
#include "tinyxml.h"


class Map3DOptions;
class Map3D : public Component, public LayerInterfaceImpl
{
	Q_OBJECT
	Q_INTERFACES(LayerInterface)
public:
	Map3D();
	~Map3D();

	void initialize();

private:
	Map3DOptions *m_map3DOptionsDlg;

public slots:
	void slotMap3DOptions();
private slots:
	void slot_extentsChanged();
	void slotLayersChanged();
	void slotLayersRemoved( const QStringList& layers);

private:
	void myRemoveLayer(QString name);
	void loadMap();
	void loadGroup(TiXmlElement *xmlNode);
	void loadLayer(TiXmlElement *xmlNode);
	QStringList getTypeLayers(QStringList layers, QString type);
	QStringList mapLayers2Layers(QMap<QString, QgsMapLayer *> mapLayers);
	QString convertEarthLayerName(QString name);

	void addSky();
};

#endif // MAP3D_H
