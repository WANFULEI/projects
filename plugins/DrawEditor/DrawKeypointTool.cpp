#include "DrawKeypointTool.h"
#include "IconDlg.h"
#include "Keypoint.h"
#include "GraphicLayer.h"
#include "component/GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "draweditor_global.h"

DrawKeypointTool::DrawKeypointTool(QString path, QWidget *parent)
{
	m_dlg = new IconDlg(parent);
	m_dlg->init(path);
	m_iconWidth = 24;
	m_iconHeight = 24;
	connect(m_dlg, SIGNAL(signSelect(QString)), this, SLOT(slotSelect(QString)));
}


DrawKeypointTool::~DrawKeypointTool(void)
{
}

void DrawKeypointTool::canvasReleaseEvent(QgsMapMouseEvent* e)
{
	if(e->button() == Qt::LeftButton){
		if(m_path.isEmpty()) return;
		GraphicLayer *layer = getActiveLayer();
		if(layer == 0) return;
		Keypoint *kp = layer->createGraphic<Keypoint>();
		kp->setIconPath(m_path);
		kp->setImage(m_cache);
		kp->setIconWidth(m_iconWidth);
		kp->setIconHeight(m_iconHeight);
		kp->setVertex(QgsPoint2Vec3d(e->mapPoint()));

		layer->clearCacheImage();
	}
}

void DrawKeypointTool::slotSelect(QString path)
{
	m_path = path;
	m_cache = QImage(m_path);
	m_image = osgDB::readImageFile(path.toStdString());

}
