#include "GlobalInstance.h"


GlobalInstance::GlobalInstance(void)
{
	m_map2D = 0;
	m_map3D = 0;
}


GlobalInstance::~GlobalInstance(void)
{
}

GlobalInstance *GlobalInstance::getInstance(){
	static GlobalInstance instance;
	return &instance;
}
