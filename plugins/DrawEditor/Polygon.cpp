#include "Polygon.h"
#include "intersection/intersection.h"
#include "component/GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "GraphicEditor.h"
#include "draweditor_global.h"

Polygon::Polygon(void)
	:GraphicItem(GraphicItem::Polygon)
{

}


Polygon::~Polygon(void)
{
}

bool Polygon::isSelect(QgsMapMouseEvent *event)
{
	QList<QPoint> points;
	auto trans = global->getMap2D()->mapSettings().mapToPixel();
	auto vertexs = getVertexs();
	for(auto i = vertexs.begin(); i != vertexs.end(); ++i){
		points << trans.transform(Vec3d2QgsPoint(*i)).toQPointF().toPoint();
	}
	intersection2::Result res = intersection2::intersection(points, event->pos(), true);
	setSelectResult(res);
	if(res.type == intersection2::None) return false;
	if(getEditor()) getEditor()->setEditingItem(this);
	return true;
}
