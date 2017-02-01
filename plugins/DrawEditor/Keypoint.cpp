#include "Keypoint.h"
#include "intersection/intersection.h"
#include "KeypointEditor.h"
#include "component/GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "qgsmapsettings.h"
#include "draweditor_global.h"

KeypointEditor *Keypoint::m_defaultEditor = new KeypointEditor;
Keypoint::Keypoint(void)
	:GraphicItem(GraphicItem::Point)
{
	setEditor(m_defaultEditor);
}

Keypoint::~Keypoint(void)
{
}

bool Keypoint::isSelect(QgsMapMouseEvent *event)
{
	auto vertexs = getVertexs();
	if(vertexs.size() == 0) return false;
	if(intersection2::intersection(event->pos(), global->getMap2D()->mapSettings().mapToPixel().transform(Vec3d2QgsPoint(vertexs[0])).toQPointF().toPoint())){
		if(getEditor()){
			getEditor()->setEditingItem(this);
			return true;
		}
	}
	return false;
}
