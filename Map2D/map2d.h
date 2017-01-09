#ifndef MAP2D_H
#define MAP2D_H

#include "map2d_global.h"
#include "component.h"
#define CORE_EXPORT __declspec(dllimport)
#define GUI_EXPORT __declspec(dllimport)
#define APP_EXPORT __declspec(dllimport)

#include "qgsmapcanvas.h"
#include "QgsLayerTreeView.h"
#include "LayerInterface.h"

class TiXmlElement;
class Map2D : public Component, public LayerInterface
{
	Q_OBJECT
	Q_INTERFACES(LayerInterface)
public:
	Map2D();
	~Map2D();

	class Tools
	{
	public:
		QgsMapTool *m_zoomIn;
        QgsMapTool *m_zoomOut;
        QgsMapTool *m_pan;
		QgsMapTool *m_select;
		QgsMapTool *m_measureDist;
		Tools(){
			m_zoomIn = 0;
			m_zoomOut = 0;
			m_pan = 0;
			m_select = 0;
			m_measureDist = 0;
		}
		~Tools(){
			
		}
	}m_tools;

protected:
	void initialize();

	void createLayerView();

private:
	void initTools();

//²Ëµ¥º¯Êý
private slots:
	void slotPan();
	void slotZoomIn();
	void slotZoomOut();
	void slotReset();
	void slotZoomToLayer();
	void slotZoomToLast();
	void slotZoomToNext();
	void slotRefresh();
	void slotMeasureDist();

private slots:
	void slotRemoveLayer();

private:
	QgsMapCanvas *m_map;
	QgsLayerTreeView *m_layerTree;
	QgsLayerTreeGroup *m_rootGroup;

private:
	void loadMap();
	void loadGroup(TiXmlElement *xmlNode, QgsLayerTreeGroup *group);
	void loadLayer(TiXmlElement *xmlNode, QgsLayerTreeGroup *group);
	QList<QgsMapLayer *> getLayers(QgsLayerTreeNode *node);

	virtual bool addRasterLayer(QString layerName, QString rasterFilePath);

};

#endif // MAP2D_H
