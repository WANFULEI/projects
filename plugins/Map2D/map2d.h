#ifndef MAP2D_H
#define MAP2D_H

#include "qmath.h"
#include "component/component.h"
#include "QgsLayerTreeView.h"
#include "LayerInterface.h"
#include "tinyxml.h"
#include "qgsmaptool.h"
#include "LayerInterfaceImpl.h"


class Map2D : public Component, public LayerInterfaceImpl
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
	void slotRightRotate();
	void slotLeftRotate();
	void slotNorth();

private slots:
	void slotRemoveLayer();

private:
	QgsLayerTreeView *m_layerTree;

private:
	void loadMap();
	void loadGroup(TiXmlElement *xmlNode, QgsLayerTreeGroup *group);
	void loadLayer(TiXmlElement *xmlNode, QgsLayerTreeGroup *group);
	QList<QgsMapLayer *> getLayers(QgsLayerTreeNode *node);

};

#endif // MAP2D_H
