#pragma once
#define CORE_EXPORT __declspec(dllimport)
#define GUI_EXPORT __declspec(dllimport)
#define APP_EXPORT __declspec(dllimport)
#include "QgsLayerTreeView.h"

class MenuProvider : public QObject, public QgsLayerTreeViewMenuProvider
{
	Q_OBJECT
public:
	MenuProvider(void);
	~MenuProvider(void);

protected:
	virtual QMenu* createContextMenu();

private slots:
	void slotAddRasterLayer();
	void slotAddVectorLayer();
	void slotAddDemLayer();
};

