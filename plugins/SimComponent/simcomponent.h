#ifndef SIMCOMPONENT_H
#define SIMCOMPONENT_H

#include "simcomponent_global.h"
#include "Component/component.h"

class SimComponent : public Component
{
public:
	SimComponent();
	~SimComponent();

	virtual void initialize();

private:

};

#endif // SIMCOMPONENT_H
