#include "GlobalInstance.h"


GlobalInstance::GlobalInstance(void)
{
	m_map2D = 0;
	m_map3D = 0;
	m_root = 0;
	m_mainWindow = 0;
	m_layerTreeRoot = 0;
	m_mapNode = 0;
	m_manipulator = 0;
	m_viewer = 0;
}


GlobalInstance::~GlobalInstance(void)
{
}

GlobalInstance *GlobalInstance::getInstance(){
	static GlobalInstance instance;
	return &instance;
}
