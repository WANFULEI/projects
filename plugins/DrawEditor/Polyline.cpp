#include "Polyline.h"
#include "PolylineEditor.h"
#include "component/GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "draweditor_global.h"

PolylineEditor *Polyline::m_defaultEditor = new PolylineEditor;
Polyline::Polyline(void)
	:GraphicItem(GraphicItem::Polyline)
{
	setEditor(m_defaultEditor);
}


Polyline::~Polyline(void)
{
}

bool Polyline::isSelect(QgsMapMouseEvent *event)
{
	QList<QPoint> points;
	auto trans = global->getMap2D()->mapSettings().mapToPixel();
	auto vertexs = getVertexs();
	for(auto i = vertexs.begin(); i != vertexs.end(); ++i){
		points << trans.transform(Vec3d2QgsPoint(*i)).toQPointF().toPoint();
	}
	intersection2::Result res = intersection2::intersection(points, event->pos(), false);
	setSelectResult(res);
	if(res.type == intersection2::None) return false;
	if(getEditor()) getEditor()->setEditingItem(this);
	return true;
}

