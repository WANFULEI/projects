#pragma once

#include "InputEvent.h"

class GraphicItem;
class GraphicEditor : public InputEvent
{
public:
	GraphicEditor(void);
	~GraphicEditor(void);

	GraphicItem *getEditingItem() const { return m_editingItem; }
	void setEditingItem(GraphicItem *graphic) { m_editingItem = graphic; }

private:
	GraphicItem *m_editingItem;
};

