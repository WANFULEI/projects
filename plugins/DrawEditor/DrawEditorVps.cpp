#include "DrawEditorVps.h"
#include "component/GlobalInstance.h"
#include "qgsmapcanvas.h"
#include "QgsMapLayerRegistry.h"
#include "GraphicLayer.h"
#include "GraphicItem.h"
#include "DrawTool.h"
#include "GraphicEditor.h"

DrawEditorVps::DrawEditorVps(QgsMapCanvas *map)
	:QgsMapTool(map)
{
	m_drawTool = 0;
	m_transTo = 0;
}


DrawEditorVps::~DrawEditorVps(void)
{
}

void DrawEditorVps::canvasMoveEvent(QgsMapMouseEvent* e)
{
	if(m_transTo) m_transTo->canvasMoveEvent(e);
}

void DrawEditorVps::canvasPressEvent(QgsMapMouseEvent* e)
{
	InputEvent *oldtransTo = m_transTo;
	m_transTo = m_drawTool;
	QStringList layers = GlobalInstance::getInstance()->getMap2D()->mapSettings().layers();
	for(auto i = layers.begin(); i != layers.end(); ++i){
		QString layer = *i;
		GraphicLayer *graphicLayer = dynamic_cast<GraphicLayer *>(QgsMapLayerRegistry::instance()->mapLayer(layer));
		if(graphicLayer == 0) continue;
		QList<GraphicItem *> gras = graphicLayer->getAll();
		for(auto j = gras.begin(); j != gras.end(); ++j){
			GraphicItem *gra = *j;
			if(gra->isSelect(e) && gra->getEditor()){
				m_transTo = gra->getEditor();
				goto out_label;	
			}
		}
	}
out_label:
	if(oldtransTo != m_transTo){
		if(oldtransTo) oldtransTo->onDeActive();
		if(m_transTo) m_transTo->onActive();
	}
	if(m_transTo) m_transTo->canvasPressEvent(e);
}

void DrawEditorVps::canvasReleaseEvent(QgsMapMouseEvent* e)
{
	if(m_transTo) m_transTo->canvasReleaseEvent(e);
}

void DrawEditorVps::keyPressEvent(QKeyEvent* e)
{
	if(m_transTo) m_transTo->keyPressEvent(e);
}

void DrawEditorVps::keyReleaseEvent(QKeyEvent* e)
{
	if(m_transTo) m_transTo->keyReleaseEvent(e);
}

void DrawEditorVps::selectTool(DrawTool *tool)
{
	if(m_drawTool == tool) return;
	if(m_drawTool) m_drawTool->onDeActive();
	m_drawTool = tool;
	if(m_drawTool) m_drawTool->onActive();
}
