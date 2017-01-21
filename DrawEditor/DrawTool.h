#pragma once

#include "InputEvent.h"

class GraphicLayer;
class DrawTool : public InputEvent
{
public:
	DrawTool(void);
	~DrawTool(void);

	GraphicLayer *getActiveLayer() const;

	virtual void onActive() {}
	virtual void onDeActive() {}
};

