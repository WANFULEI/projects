#include "KeypointEditor.h"
#include "GraphicItem.h"
#include "component/GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "graphicLayer.h"
#include "MyItem.h"


KeypointEditor::KeypointEditor(void)
{
}


KeypointEditor::~KeypointEditor(void)
{
}

void KeypointEditor::canvasMoveEvent(QgsMapMouseEvent* e)
{
	
}

void KeypointEditor::canvasPressEvent(QgsMapMouseEvent* e)
{
// 	MyItem *myItem = new MyItem(global->getMap2D());
// 	myItem->m_pt = e->mapPoint();
// 	global->getMap2D()->scene()->addItem(myItem);
}

void KeypointEditor::canvasReleaseEvent(QgsMapMouseEvent* e)
{
	if(e->button() == Qt::LeftButton){
		getEditingItem()->setVertex(osg::Vec3d(e->mapPoint().x(), e->mapPoint().y(), 0));
		getEditingItem()->getLayer()->clearCacheImage();
	}
}
